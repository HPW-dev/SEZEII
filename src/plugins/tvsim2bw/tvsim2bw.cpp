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
  decode_stream(*bw_img_scaled);
  upscale();
  convert_to_dst(dst);
}

void Tvsim2bw::downscale() {
  bw_img_scaled->init(scale_wh.x, scale_wh.y, seze_f_gray);
  if (use_scale)
    scale_gray(*bw_img, *bw_img_scaled, scale_type_in);
  else
    std::swap(bw_img_scaled, bw_img);
}

void Tvsim2bw::upscale() {
  if (use_scale)
    scale_gray(*bw_img_scaled, *bw_img, scale_type_out);
  else
    std::swap(bw_img_scaled, bw_img);
}

void Tvsim2bw::desaturate_(CN(seze::Image) src) {
  bw_img->init(src.X, src.Y, seze_f_gray);
  desaturate(src, *bw_img, desat_type);
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
  FOR (_, vfront) {
    stream[i] = beam_off_signal;
    ++i;
  }

  FOR (y, src.Y) {
    if (interlacing && ((y + int(is_odd_str)) & 1))
      continue;
    // hfront: off
    FOR (_, hfront) {
      stream[i] = beam_off_signal;
      ++i;
    }
    // src.X:pix
    FOR (x, src.X) {
      stream[i] = encode_pix(src.fast_get<luma_t>(x, y));
      ++i;
    }
    // hback:off
    FOR (_, hback) {
      stream[i] = beam_off_signal;
      ++i;
    }
    // hsync:sync
    FOR (_, hsync_sz) {
      stream[i] = sync_signal;
      ++i;
    }
  } // for src.Y

  // vback:off
  FOR (_, vback) {
    stream[i] = beam_off_signal;
    ++i;
  }
  // vsync:sync
  FOR (_, vsync_sz) {
    stream[i] = sync_signal;
    ++i;
  }

  is_odd_str = !is_odd_str;
} // encode_stream

void Tvsim2bw::decode_stream(seze::Image &dst) {
  std::fill(dst.get_data(), dst.get_data() + dst.bytes, 0);
  bool new_str {false};
  for (CN(auto) signal: stream) {
    // sync
    if (signal <= sync_lvl) {
      // hsync
      beam.x = std::max<real>(-hfront, beam.x - beam_spd_back);
      new_str = true;
      ++sync_cnt;
      // vsync
      if (sync_cnt >= vsync_needed_cnt)
        beam.y = std::max<real>(-vfront, beam.y - beam_spd_back);
    } else { // pix
      sync_cnt = 0;
      if (new_str) {
        new_str = false;
        beam.y = std::min(beam.y + beam_spd_y, 3'000.0f);
      }
      if (signal >= black_lvl) {
        auto pos_y {beam.y + vfront};
        if (interlacing)
          pos_y = (pos_y * 2) + int(is_odd_str);
        dst.set<luma_t>(beam.x, pos_y, decode_pix(signal));
      }
      beam.x = std::min(beam.x + beam_spd_x, 3'000.0f);
    } // else pix
  }
  display_simul(dst);
} // decode_stream

void Tvsim2bw::resize_stream(CN(seze::Image) src) {
  str_sz = src.X + hfront + hback + hsync_sz;
  const size_t str_count = interlacing ? src.Y / 2 : src.Y;
  const size_t frame_sz = (str_count * str_sz) + vfront + vback + vsync_sz;
  stream.resize(frame_sz);
}

real Tvsim2bw::encode_pix(luma_t src) const
  { return std::lerp(black_lvl, white_lvl, src); }

luma_t Tvsim2bw::decode_pix(real src) const
 { return to_range(src, black_lvl, white_lvl, 0.0f, 1.0f); }

void Tvsim2bw::update() {
  if (fix_opts) {
    hsync_sz = std::clamp(hsync_sz, 1, vsync_sz-1);
    vsync_sz = std::clamp(vsync_sz, 3, 7'000);
    vsync_needed_cnt = std::clamp(vsync_needed_cnt, hsync_sz+1, vsync_sz-1);
    black_lvl = std::clamp(black_lvl, -2.0f, white_lvl);
    beam_off_signal = std::clamp(beam_off_signal, -2.0f, black_lvl);
    sync_lvl = std::clamp(sync_lvl, -2.0f, beam_off_signal);
    sync_signal = std::clamp(sync_signal, -2.0f, sync_lvl);
  } // if fix_opts
} // update

void Tvsim2bw::display_simul(seze::Image &dst) {
  return_if (!use_fading);
  display->init(dst.X, dst.Y, seze_f_gray);
  FOR (i, display->size) {
    auto &pix = display->fast_get<luma_t>(i);
    pix = std::max<luma_t>(0, pix - fading);
    pix = std::max<luma_t>(pix, dst.fast_get<luma_t>(i));
  }
  display->fast_copy_to(dst);
}
