#pragma once
#include <cassert>
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

seze::YUVf RGB24_to_yuv(CN(seze::RGB24) c);
seze::RGB24 yuv_to_RGB24(CN(seze::YUVf) c);

//! алгоритм преобразования диапазона (map arduino)
inline real to_range(real x, real in_min, real in_max,
real out_min, real out_max)
  { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

constexpr int align_by(int x, int aligment) {
  assert(aligment > 0);
  return int(double(x) / aligment + 0.5) * aligment;
}

inline void filter_sharp(auto &stream, real power=1) {
  return_if (power == 0);
  const std::array<real, 3> sharp_mtx {-power, 1 + power * 2, -power};
  const auto size {std::size(stream)};
  assert(size > 2);
  auto buf = stream;
  for (size_t i {1}; i < size; ++i) {
    stream[i] =
      buf[i - 1] * sharp_mtx[0] +
      buf[i    ] * sharp_mtx[1] +
      buf[i + 1] * sharp_mtx[2];
  }
} // filter_sharp