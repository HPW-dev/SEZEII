#pragma once
#include "utils/macro.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

void gray_to_rgb24(CN(seze::Image) src, seze::Image &dst);
void desaturate(CN(seze::Image) src, seze::Image &dst,
  desaturation_e type);
void scale_gray(CN(seze::Image) src, seze::Image &dst,
  scale_e type = scale_e::bilinear);
void filtering(vector_t<luma_t> &stream, int power = 3,
  filter_e type = filter_e::average_fast);
void apply_noise(vector_t<luma_t> &stream, real noise_level);

//! алгоритм преобразования диапазона (map arduino)
inline real to_range(real x, real in_min, real in_max,
real out_min, real out_max)
  { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }
