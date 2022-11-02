#pragma once
#include <algorithm>
#include "utils/macro.hpp"
#include "utils/types.hpp"

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
  bool operator < (CN(RGB_base<T2>) other) const {
    return (int(int(int(R << 8) | G) << 8 ) | B)
    < (int(int(int(other.R << 8) | other.G) << 8 ) | other.B);
  }
  
  template <typename T2>
  RGB_base<T> operator - (CN(RGB_base<T2>) other) const {
    return RGB_base<T>(
      std::clamp(int(R - other.R), 0, 255),
      std::clamp(int(G - other.G), 0, 255),
      std::clamp(int(B - other.B), 0, 255)
    );
  }

  template <typename T2>
  RGB_base<T> operator + (CN(RGB_base<T2>) other) const {
    return RGB_base<T>(
      std::clamp(int(R + other.R), 0, 255),
      std::clamp(int(G + other.G), 0, 255),
      std::clamp(int(B + other.B), 0, 255)
    );
  }
  
  template <typename T2>
  void operator ^= (CN(RGB_base<T2>) other) {
    R = int(R) ^ int(other.R);
    G = int(G) ^ int(other.G);
    B = int(B) ^ int(other.B);
  }

  template <typename T2>
  void operator += (CN(RGB_base<T2>) other) {
    R = std::clamp(int(R + other.R), 0, 255);
    G = std::clamp(int(G + other.G), 0, 255);
    B = std::clamp(int(B + other.B), 0, 255);
  }

  template <typename T2>
  void operator += (T2 other) {
    R = std::clamp(int(R + other), 0, 255);
    G = std::clamp(int(G + other), 0, 255);
    B = std::clamp(int(B + other), 0, 255);
  }
}; // RGB24

using RGB24 = RGB_base<byte>;
using RGBi = RGB_base<int>;
using RGBf = RGB_base<float>;
using RGBld = RGB_base<ldouble>;

} // seze ns
