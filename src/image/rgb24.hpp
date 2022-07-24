#pragma once
#include <algorithm>
#include "utils/macro.hpp"

namespace seze {

template <typename T>
struct RGB_base {
public:
  T R = 0, G = 0, B = 0;
  constexpr RGB_base(T r=0, T g=0, T b=0): R(r), G(g), B(b) {}

  template <typename T2>
  bool operator ==(CN(RGB_base<T2>) other) const
    { return R == other.R && G == other.G && B == other.B; }
  
  template <typename T2>
  bool operator !=(CN(RGB_base<T2>) other) const
    { return R != other.R || G != other.G || B != other.B; }
  
  template <typename T2>
  bool operator < (CN(RGB_base<T2>) other) const
    { return R < other.R && G < other.G && B < other.B; }
  
  template <typename T2>
  RGB_base<T> operator - (CN(RGB_base<T2>) other) const
    { return RGB_base<T>(std::max(int(R - other.R), 0), std::max(int(G - other.G), 0), std::max(int(B - other.B), 0)); }
  
  template <typename T2>
  RGB_base<T> operator + (CN(RGB_base<T2>) other) const
    { return RGB_base<T>(std::min(int(R + other.R), 255), std::min(int(G + other.G), 255), std::min(int(B + other.B), 255)); }
}; // RGB24

using RGB24 = RGB_base<byte>;
using RGBi = RGB_base<int>;
using RGBf = RGB_base<float>;
using RGBld = RGB_base<ldouble>;

} // seze ns
