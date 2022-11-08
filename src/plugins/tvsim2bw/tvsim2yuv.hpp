#pragma once
#include "tvsim2bw.hpp"

class Tvsim2yuv: public Tvsim2bw {
  nocopy(Tvsim2yuv);

  shared_p<seze::Image> u_img {};
  shared_p<seze::Image> v_img {};
  shared_p<seze::Image> u_img_scaled {};
  shared_p<seze::Image> v_img_scaled {};
  v_luma_t u_stream {};
  v_luma_t v_stream {};

  void split_channels(CN(seze::Image) src);
  void combine_channels(seze::Image &dst);

public:
  Tvsim2yuv();
  ~Tvsim2yuv() = default;


  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst) override;
}; // Tvsim2bw
