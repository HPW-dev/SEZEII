#include <algorithm>
#include <cmath>
#include "tvsim2bw.hpp"
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

Tvsim2bw::Tvsim2bw()
: bw_img {make_shared_p<seze::Image>()}
, bw_img_scaled {make_shared_p<seze::Image>()}
, display {make_shared_p<seze::Image>()}
{}

void Tvsim2bw::operator ()(CN(seze::Image) src, seze::Image &dst) {
  update();
  desaturate_(src);
  downscale();
  encode_stream(*bw_img_scaled);
  apply_noise(stream, conf.noise_level);
  ringing(stream, conf.ringing_ratio, conf.ringing_len, conf.ringing_power);
  filtering(stream, conf.filter_power, conf.filter_type);
  decode_stream(*bw_img_scaled);
  upscale();
  convert_to_dst(dst);
}

void Tvsim2bw::downscale() {
  bw_img_scaled->init(conf.scale_wh.x, conf.scale_wh.y, seze_f_gray);
  if (conf.use_scale)
    scale_gray(*bw_img, *bw_img_scaled, conf.scale_type_in);
  else
    std::swap(bw_img_scaled, bw_img);
}

void Tvsim2bw::upscale() {
  if (conf.use_scale)
    scale_gray(*bw_img_scaled, *bw_img, conf.scale_type_out);
  else
    std::swap(bw_img_scaled, bw_img);
}

void Tvsim2bw::desaturate_(CN(seze::Image) src) {
  bw_img->init(src.X, src.Y, seze_f_gray);
  desaturate(src, *bw_img, conf.desat_type);
}

void Tvsim2bw::convert_to_dst(seze::Image &dst) {
  gray_to_rgb24(*bw_img, dst);
}

void Tvsim2bw::encode_stream(CN(seze::Image) src) {
  resize_stream(src);
  size_t i {0}; // stream idx

  /* tv signal map:
  v signal:
    vfront:off
  h signal:
    hfront:off, src.X:pix, hback:off, hsync:sync
  v signal:
    vback:off, vsync:sync */

  // vfront:off
  FOR (_, conf.vfront) {
    stream[i] = conf.beam_off_signal;
    ++i;
  }

  FOR (y, src.Y) {
    if (conf.interlacing && ((y + int(is_odd_str)) & 1))
      continue;
    // hfront: off
    FOR (_, conf.hfront) {
      stream[i] = conf.beam_off_signal;
      ++i;
    }
    // src.X:pix
    FOR (x, src.X) {
      stream[i] = encode_pix(src.fast_get<luma_t>(x, y));
      ++i;
    }
    // hback:off
    FOR (_, conf.hback) {
      stream[i] = conf.beam_off_signal;
      ++i;
    }
    // hsync:sync
    FOR (_, conf.hsync_sz) {
      stream[i] = conf.sync_signal;
      ++i;
    }
  } // for src.Y

  // vback:off
  FOR (_, conf.vback) {
    stream[i] = conf.beam_off_signal;
    ++i;
  }
  // vsync:sync
  FOR (_, conf.vsync_sz) {
    stream[i] = conf.sync_signal;
    ++i;
  }

  is_odd_str = !is_odd_str;
} // encode_stream

void Tvsim2bw::decode_stream(seze::Image &dst) {
  std::fill(dst.get_data(), dst.get_data() + dst.bytes, 0);
  bool new_str {false};
  for (CN(auto) signal: stream) {
    // sync
    if (signal <= conf.sync_lvl) {
      // hsync
      beam.x = std::max<real>(-conf.hfront, beam.x - conf.beam_spd_back);
      new_str = true;
      ++sync_cnt;
      // vsync
      if (sync_cnt >= conf.vsync_needed_cnt)
        beam.y = std::max<real>( -conf.vfront,
          beam.y - conf.beam_spd_back);
    } else { // pix
      sync_cnt = 0;
      if (new_str) {
        new_str = false;
        beam.y = std::min(beam.y + conf.beam_spd_y, 3'000.0f);
      }
      if (signal >= conf.black_lvl) {
        auto pos_y {beam.y + conf.vfront};
        if (conf.interlacing)
          pos_y = (pos_y * 2) + int(is_odd_str);
        dst.set<luma_t>(beam.x, pos_y, decode_pix(signal) * conf.amp);
      }
      beam.x = std::min(beam.x + conf.beam_spd_x, 3'000.0f);
    } // else pix
  }
  display_simul(dst);
} // decode_stream

void Tvsim2bw::resize_stream(CN(seze::Image) src) {
  str_sz = src.X + conf.hfront + conf.hback + conf.hsync_sz;
  const size_t str_count = conf.interlacing ? src.Y / 2 : src.Y;
  const size_t frame_sz = (str_count * str_sz) + conf.vfront
    + conf.vback + conf.vsync_sz;
  stream.resize(frame_sz);
}

real Tvsim2bw::encode_pix(luma_t src) const
  { return std::lerp(conf.black_lvl, conf.white_lvl, src); }

luma_t Tvsim2bw::decode_pix(real src) const
 { return to_range(src, conf.black_lvl, conf.white_lvl, 0.0f, 1.0f); }

void Tvsim2bw::update() {
  if (conf.fix_opts) {
    conf.hsync_sz = std::clamp(conf.hsync_sz, 1, conf.vsync_sz-1);
    conf.vsync_sz = std::clamp(conf.vsync_sz, 3, 7'000);
    conf.vsync_needed_cnt = std::clamp(conf.vsync_needed_cnt,
      conf.hsync_sz+1, conf.vsync_sz-1);
    conf.black_lvl = std::clamp(conf.black_lvl, -2.0f, conf.white_lvl);
    conf.beam_off_signal = std::clamp(conf.beam_off_signal, -2.0f,
      conf.black_lvl);
    conf.sync_lvl = std::clamp(conf.sync_lvl, -2.0f, conf.beam_off_signal);
    conf.sync_signal = std::clamp(conf.sync_signal, -2.0f, conf.sync_lvl);
  } // if fix_opts
} // update

void Tvsim2bw::display_simul(seze::Image &dst) {
  return_if (!conf.use_fading);
  display->init(dst.X, dst.Y, seze_f_gray);
  #pragma omp parallel for simd
  FOR (i, display->size) {
    auto &pix {display->fast_get<luma_t>(i)};
    pix = std::max<luma_t>(0, pix - conf.fading);
    pix = std::max<luma_t>(pix, dst.fast_get<luma_t>(i));
  }
  display->fast_copy_to(dst);
}

void Tvsim2bw::ringing(std::vector<luma_t> &stream, real ratio, int len,
real power) {
  return_if ( !conf.use_ringing);
  return_if (ratio <= 0);
  return_if (len <= 0);
  return_if (power <= 0);
  for (uint i {0}; auto& signal: stream) {
    luma_t ret {0};
    FOR (j, len) {
      auto idx = i + j;
      if (idx >= stream.size())
        break;
      auto alpha = std::lerp(1.0f, 0.0f, j / real(len));
      auto mul = std::cos(ratio * j) * alpha;
      ret += stream[idx] * mul * power;
    }
    signal = ret;
    ++i;
  }
} // ringing
