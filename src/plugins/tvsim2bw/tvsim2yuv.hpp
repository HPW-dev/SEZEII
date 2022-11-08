#pragma once
#include "tvsim2bw.hpp"

class Tvsim2yuv: public Tvsim2bw {
  nocopy(Tvsim2yuv);

  shared_p<seze::Image> u_img {};
  shared_p<seze::Image> v_img {};
  shared_p<seze::Image> u_img_scaled {};
  shared_p<seze::Image> v_img_scaled {};
  shared_p<seze::Image> u_display {};
  shared_p<seze::Image> v_display {};
  shared_p<seze::Image> rgb_display {};
  v_luma_t u_stream {};
  v_luma_t v_stream {};

  void split_channels(CN(seze::Image) src);
  void combine_channels(seze::Image &dst);
  void downscale() override;
  void upscale() override;
  void encode_stream_uv(CN(seze::Image) src_u, CN(seze::Image) src_v);
  void decode_stream_yuv(seze::Image &dst_y, seze::Image &dst_u, seze::Image &dst_v);
  void display_simul_yuv(seze::Image &dst_y, seze::Image &dst_u, seze::Image &dst_v);
  void display_simul_rgb(seze::Image &dst);

public:
  Tvsim2yuv();
  ~Tvsim2yuv() = default;


  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst) override;
}; // Tvsim2bw
