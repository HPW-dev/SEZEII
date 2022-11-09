#pragma once
#include "utils/macro.hpp"
#include "types.hpp"
#include "image/rgb24.hpp"
#include "image/YUV.hpp"

namespace seze {
  class Image;
}

void gray_to_rgb24(CN(seze::Image) src, seze::Image &dst);
void desaturate(CN(seze::Image) src, seze::Image &dst,
  desaturation_e type);
void scale_gray(CN(seze::Image) src, seze::Image &dst,
  scale_e type = scale_e::bilinear);
void filtering(v_luma_t &stream, int power = 3,
  filter_e type = filter_e::average_fast);
void apply_noise(v_luma_t &stream, real noise_level);

//! алгоритм преобразования диапазона (map arduino)
inline real to_range(real x, real in_min, real in_max,
real out_min, real out_max)
  { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

seze::YUVf RGB24_to_yuv(CN(seze::RGB24) c);
seze::RGB24 yuv_to_RGB24(CN(seze::YUVf) c);