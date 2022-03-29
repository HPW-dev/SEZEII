#pragma once
#include "utils/macro.hpp"

namespace seze {

template <typename T> struct YUV {
public:
  T Y = 0, U = 0, V = 0;
  YUV(T y=0, T u=0, T v=0): Y(y), U(u), V(v) {}
}; // YUV24

using YUV24 = YUV<byte>;
using YUVf = YUV<float>;
using YUVld = YUV<ldouble>;

} // seze ns

