#include <numbers>
#include <algorithm>
#include <cmath>
#include "tvsim2bw.hpp"
#include "util.hpp"
#include "image/image.hpp"

Tvsim2bw::Tvsim2bw()
: bw_img {make_shared_p<seze::Image>()}
, bw_img_scaled {make_shared_p<seze::Image>()}
, display {make_shared_p<seze::Image>()}
{
  title = "TV sim. 2 (BW)";
}

void Tvsim2bw::operator ()(CN(seze::Image) src, seze::Image &dst) {
  update();
  desaturate_(src);
  downscale();
  encode_stream(*bw_img_scaled);
  amplify(conf.pre_amp);
  if (conf.use_am)
    am_modulate(stream, conf.noise_level, conf.filter_power);
  else {
    apply_noise(stream, conf.noise_level);
    filtering(stream, conf.filter_power, conf.filter_type);
  }
  amplify(conf.amp);
  decode_stream(*bw_img_scaled);
  upscale();
  convert_to_dst(dst);
} // op ()

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
  if ( !conf.debug_black_bg)
    gray_to_rgb24(*bw_img, dst);
  else
    std::fill(dst.get_data(), dst.get_data() + dst.bytes, 0);
  if (conf.debug)
    draw_debug(dst);
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
        dst.set<luma_t>(beam.x, pos_y, decode_pix(signal));
      }
      beam.x = std::min(beam.x + conf.beam_spd_x, 3'000.0f);
    } // else pix
  }
  display_simul(dst);
} // decode_stream

size_t Tvsim2bw::resize_stream(CN(seze::Image) src) {
  str_sz = src.X + conf.hfront + conf.hback + conf.hsync_sz;
  const size_t str_count = conf.interlacing ? src.Y / 2 : src.Y;
  const size_t frame_sz = (str_count * str_sz) + conf.vfront
    + conf.vback + conf.vsync_sz;
  stream.resize(frame_sz);
  return frame_sz;
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

void Tvsim2bw::am_modulate(v_luma_t &dst, real noise_level, int filter_power) {
  const auto freg {conf.am_freg};
  const auto depth {conf.am_depth};
  // modulate
  #pragma omp parallel for simd
  FOR (i, dst.size()) {
    auto &x {dst[i]};
    const auto at {1.0f + depth * x};
    const auto st {at * std::cos(freg * i)};
    x = st;
  }
  // noise
  apply_noise(dst, noise_level);
  // demodulate
  buf_a.resize(dst.size());
  buf_b.resize(dst.size());
  const auto pre_mul {std::numbers::pi * 0.5f};
  #pragma omp parallel for simd
  FOR (i, dst.size()) {
    const auto g {std::cos(freg * i)};
    const auto xg {dst[i] * g};
    buf_a[i] = xg;
    buf_b[i] = xg * pre_mul;
  }
  filtering(buf_a, filter_power, conf.filter_type);
  filtering(buf_b, filter_power, conf.filter_type);
  const auto tune {conf.am_tune};
  #pragma omp parallel for simd
  FOR (i, dst.size()) {
    auto a = buf_a[i];
    a *= a;
    auto b = buf_b[i];
    b *= b;
    dst[i] = std::sqrt(a + b) - tune;
  }
} // modulate

void Tvsim2bw::draw_debug(seze::Image &dst) const {
  real y_old {0};
  FOR (x, dst.X) {
    auto y = stream[x];
    y = 2.0f - y;
    y *= dst.Y / 4.0f;
    y = std::clamp<real>(y, 0, dst.Y);
    auto st = std::min(y, y_old);
    auto ed = std::max(y, y_old);
    for (int i = std::floor(st); i < std::floor(ed+1); ++i) {
      auto &col = dst.fast_get<seze::RGB24>(x, i);
      col ^= seze::RGB24(255,255,255);
    }
    y_old = y;
  }
} // draw_debug

void Tvsim2bw::amplify(real amp) {
  return_if(amp == 1);
  #pragma omp for simd
  FOR (i, stream.size())
    stream[i] *= amp;
}
