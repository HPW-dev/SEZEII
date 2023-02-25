#include <span>
#include "tvsim2yuv.hpp"
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

Tvsim2yuv::Tvsim2yuv()
: Tvsim2bw()
, u_img {make_shared_p<seze::Image>()}
, v_img {make_shared_p<seze::Image>()}
, u_img_scaled {make_shared_p<seze::Image>()}
, v_img_scaled {make_shared_p<seze::Image>()}
, u_display {make_shared_p<seze::Image>()}
, v_display {make_shared_p<seze::Image>()}
, rgb_display {make_shared_p<seze::Image>()}
{
  title = "TV sim. 2 (Colored)";
}

void Tvsim2yuv::operator ()(CN(seze::Image) src, seze::Image &dst) {
  update();
  split_channels(src);
  downscale();
  encode_stream(*bw_img_scaled);
  encode_stream_uv(*u_img_scaled, *v_img_scaled);
  amplify(conf.pre_amp);
  if (conf.use_am) {
    am_modulate(stream, conf.noise_level, conf.filter_power);
    am_modulate(u_stream, conf_yuv.noise_uv, conf_yuv.filter_power_uv);
    am_modulate(v_stream, conf_yuv.noise_uv, conf_yuv.filter_power_uv);
  } else {
    apply_noise(stream, conf.noise_level);
    filtering(stream, conf.filter_power, conf.filter_type);
    apply_noise(u_stream, conf_yuv.noise_uv);
    filtering(u_stream, conf_yuv.filter_power_uv, conf.filter_type);
    apply_noise(v_stream, conf_yuv.noise_uv);
    filtering(v_stream, conf_yuv.filter_power_uv, conf.filter_type);
  }
  filter_sharp(v_stream, conf.sharp_power);
  filter_sharp(u_stream, conf.sharp_power);
  filter_sharp(stream, conf.sharp_power);
  amplify(conf.amp);
  if (conf.debug_black_bg) {
    std::fill(dst.get_data(), dst.get_data() + dst.bytes, 0);
  } else {
    decode_stream_yuv(*bw_img_scaled, *u_img_scaled, *v_img_scaled);
    upscale();
    combine_channels(dst);
    //sharpen(dst);
    display_simul_rgb(dst);
  }
  if (conf.debug)
    draw_debug(dst);
} // op ()

void Tvsim2yuv::split_channels(CN(seze::Image) src) {
  bw_img->init(src.X, src.Y, seze_f_gray);
  u_img->init(src.X, src.Y, seze_f_gray);
  v_img->init(src.X, src.Y, seze_f_gray);
  #pragma omp parallel for simd
  cfor (i, src.size) {
    auto src_pix {src.fast_get<seze::RGB24>(i)};
    auto yuv {RGB24_to_yuv(src_pix)};
    bw_img->fast_set<luma_t>(i, yuv.Y);
    u_img->fast_set<luma_t>(i, yuv.U);
    v_img->fast_set<luma_t>(i, yuv.V);
  }
}

void Tvsim2yuv::combine_channels(seze::Image &dst) {
  #pragma omp parallel for simd
  cfor (i, bw_img->size) {
    auto y {bw_img->fast_get<luma_t>(i)};
    auto u {u_img->fast_get<luma_t>(i)};
    auto v {v_img->fast_get<luma_t>(i)};
    dst.fast_set<seze::RGB24>(i, yuv_to_RGB24(seze::YUVf(y, u, v)));
  }
}

void Tvsim2yuv::downscale() {
  bw_img_scaled->init(conf.scale_wh.x, conf.scale_wh.y, seze_f_gray);
  u_img_scaled->init(conf.scale_wh.x, conf.scale_wh.y, seze_f_gray);
  v_img_scaled->init(conf.scale_wh.x, conf.scale_wh.y, seze_f_gray);
  if (conf.use_scale) {
    scale_gray(*bw_img, *bw_img_scaled, conf.scale_type_in);
    scale_gray(*u_img, *u_img_scaled, conf.scale_type_in);
    scale_gray(*v_img, *v_img_scaled, conf.scale_type_in);
  } else {
    std::swap(bw_img_scaled, bw_img);
    std::swap(u_img_scaled, u_img);
    std::swap(v_img_scaled, v_img);
  }
} // downscale

void Tvsim2yuv::upscale() {
  if (conf.use_scale) {
    scale_gray(*bw_img_scaled, *bw_img, conf.scale_type_out);
    scale_gray(*u_img_scaled, *u_img, conf.scale_type_out);
    scale_gray(*v_img_scaled, *v_img, conf.scale_type_out);
  } else {
    std::swap(bw_img_scaled, bw_img);
    std::swap(u_img_scaled, u_img);
    std::swap(v_img_scaled, v_img);
  }
} // upscale

void Tvsim2yuv::encode_stream_uv(CN(seze::Image) src_u, CN(seze::Image) src_v) {
  is_odd_str = !is_odd_str; // для фикса интерлейсинга цвета
  auto stream_sz {resize_stream(src_u)};
  u_stream.resize(stream_sz);
  v_stream.resize(stream_sz);
  size_t i {0}; // stream idx

  /* tv signal map:
  v signal:
    vfront:off
  h signal:
    hfront:off, src.X:pix, hback:off, hsync:off
  v signal:
    vback:off, vsync:off */

  // vfront:off
  cfor (_, conf.vfront) {
    u_stream[i] = conf.beam_off_signal;
    v_stream[i] = conf.beam_off_signal;
    ++i;
  }

  cfor (y, src_u.Y) {
    if (conf.interlacing && ((y + int(is_odd_str)) & 1))
      continue;
    // hfront: off
    cfor (_, conf.hfront) {
      u_stream[i] = conf.beam_off_signal;
      v_stream[i] = conf.beam_off_signal;
      ++i;
    }
    // src.X:pix
    cfor (x, src_u.X) {
      u_stream[i] = encode_pix(src_u.fast_get<luma_t>(x, y));
      v_stream[i] = encode_pix(src_v.fast_get<luma_t>(x, y));
      ++i;
    }
    // hback:off
    cfor (_, conf.hback) {
      u_stream[i] = conf.beam_off_signal;
      v_stream[i] = conf.beam_off_signal;
      ++i;
    }
    // hsync:off
    cfor (_, conf.hsync_sz) {
      u_stream[i] = conf.beam_off_signal;
      v_stream[i] = conf.beam_off_signal;
      ++i;
    }
  } // for src.Y

  // vback:off
  cfor (_, conf.vback) {
    u_stream[i] = conf.beam_off_signal;
    v_stream[i] = conf.beam_off_signal;
    ++i;
  }
  // vsync:off
  cfor (_, conf.vsync_sz) {
    u_stream[i] = conf.beam_off_signal;
    v_stream[i] = conf.beam_off_signal;
    ++i;
  }

  is_odd_str = !is_odd_str;
} // encode_stream_uv

void Tvsim2yuv::decode_stream_yuv(seze::Image &dst_y, seze::Image &dst_u, seze::Image &dst_v) {
  std::fill(dst_y.get_data(), dst_y.get_data() + dst_y.bytes, 0);
  std::fill(dst_u.get_data(), dst_u.get_data() + dst_u.bytes, 0);
  std::fill(dst_v.get_data(), dst_v.get_data() + dst_v.bytes, 0);
  bool new_str {false};
  for (int idx {0}; CN(auto) signal: stream) {
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
        dst_y.set<luma_t>(beam.x, pos_y, decode_pix(signal));
        dst_u.set<luma_t>(beam.x + conf_yuv.shift_u, pos_y, decode_pix(u_stream[idx]));
        dst_v.set<luma_t>(beam.x + conf_yuv.shift_v, pos_y, decode_pix(v_stream[idx]));
      }
      beam.x = std::min(beam.x + conf.beam_spd_x, 3'000.0f);
    } // else pix
    ++idx;
  }
} // decode_stream_yuv

void Tvsim2yuv::display_simul_rgb(seze::Image &dst) {
  return_if (!conf.use_fading);
  rgb_display->init(dst.X, dst.Y, seze_RGB24);
  #pragma omp parallel for simd
  cfor (i, rgb_display->size) {
    auto &pix {rgb_display->fast_get<seze::RGB24>(i)};
    CN(auto) dst_pix {dst.fast_get<seze::RGB24>(i)};
    pix.R = std::max<int>(0, pix.R - conf.fading * 255.0f);
    pix.R = std::max(pix.R, dst_pix.R);
    pix.G = std::max<int>(0, pix.G - conf.fading * 255.0f);
    pix.G = std::max(pix.G, dst_pix.G);
    pix.B = std::max<int>(0, pix.B - conf.fading * 255.0f);
    pix.B = std::max(pix.B, dst_pix.B);
  }
  rgb_display->fast_copy_to(dst);
}

void Tvsim2yuv::amplify(real amp) {
  if (amp != 1) {
    #pragma omp for simd
    cfor (i, stream.size())
      stream[i] *= amp;
  }
  if (conf_yuv.amp_u != 1) {
    #pragma omp for simd
    cfor (i, u_stream.size())
      u_stream[i] *= conf_yuv.amp_u;
  }
  if (conf_yuv.amp_v != 1) {
    #pragma omp for simd
    cfor (i, v_stream.size())
      v_stream[i] *= conf_yuv.amp_v;
  }
} // amplify
