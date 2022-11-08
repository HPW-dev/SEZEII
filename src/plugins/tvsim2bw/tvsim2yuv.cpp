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
{
  title = "TV sim. 2 (Colored)";
}

void Tvsim2yuv::operator ()(CN(seze::Image) src, seze::Image &dst) {
  update();
  split_channels(src);
  downscale();
  encode_stream(*bw_img_scaled);
  amplify(conf.pre_amp);
  if (conf.use_am)
    am_modulate();
  else {
    apply_noise(stream, conf.noise_level);
    filtering(stream, conf.filter_power, conf.filter_type);
  }
  amplify(conf.amp);
  decode_stream(*bw_img_scaled);
  upscale();
  combine_channels(dst);
}

void Tvsim2yuv::split_channels(CN(seze::Image) src) {
  bw_img->init(src.X, src.Y, seze_f_gray);
  u_img->init(src.X, src.Y, seze_f_gray);
  v_img->init(src.X, src.Y, seze_f_gray);
  FOR (i, src.size) {
    auto src_pix {src.fast_get<seze::RGB24>(i)};
    auto yuv {RGB24_to_yuv(src_pix)};
    bw_img->fast_set<luma_t>(i, yuv.Y);
    u_img->fast_set<luma_t>(i, yuv.U);
    v_img->fast_set<luma_t>(i, yuv.V);
  }
}

void Tvsim2yuv::combine_channels(seze::Image &dst) {
  FOR (i, bw_img->size) {
    auto y {bw_img->fast_get<luma_t>(i)};
    auto u {u_img->fast_get<luma_t>(i)};
    auto v {v_img->fast_get<luma_t>(i)};
    dst.fast_set<seze::RGB24>(i, yuv_to_RGB24(seze::YUVf(y, u, v)));
  }
}

