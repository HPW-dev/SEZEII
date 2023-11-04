#pragma once
#include <algorithm>
#include "utils/macro.hpp"
#include "utils/types.hpp"

namespace seze {

template <typename T, int max_val=255>
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
    if constexpr (std::is_integral_v<T>) {
      return RGB_base<T>(
        std::clamp<int>(int(R) - other.R, 0, max_val),
        std::clamp<int>(int(G) - other.G, 0, max_val),
        std::clamp<int>(int(B) - other.B, 0, max_val)
      );
    } else if (std::is_floating_point_v<T>) {
      return RGB_base<T>(
        std::clamp<double>(double(R) - other.R, 0, max_val),
        std::clamp<double>(double(G) - other.G, 0, max_val),
        std::clamp<double>(double(B) - other.B, 0, max_val)
      );
    }
  } // op -

  template <typename T2>
  RGB_base<T> operator + (CN(RGB_base<T2>) other) const {
    if constexpr (std::is_integral_v<T>) {
      return RGB_base<T>(
        std::clamp<int>(int(R) + other.R, 0, max_val),
        std::clamp<int>(int(G) + other.G, 0, max_val),
        std::clamp<int>(int(B) + other.B, 0, max_val)
      );
    } else if (std::is_floating_point_v<T>) {
      return RGB_base<T>(
        std::clamp<double>(double(R) + other.R, 0, max_val),
        std::clamp<double>(double(G) + other.G, 0, max_val),
        std::clamp<double>(double(B) + other.B, 0, max_val)
      );
    }
  }

  template <typename T2>
  RGB_base<T> operator * (CN(RGB_base<T2>) other) const {
    if constexpr (std::is_integral_v<T>) {
      return RGB_base<T>(
        std::clamp<int>(int(R) * other.R, 0, max_val),
        std::clamp<int>(int(G) * other.G, 0, max_val),
        std::clamp<int>(int(B) * other.B, 0, max_val)
      );
    } else if (std::is_floating_point_v<T>) {
      return RGB_base<T>(
        std::clamp<double>(double(R) * other.R, 0, max_val),
        std::clamp<double>(double(G) * other.G, 0, max_val),
        std::clamp<double>(double(B) * other.B, 0, max_val)
      );
    }
  }

  template <typename T2>
  RGB_base<T> operator * (T2 other) const {
    if constexpr (std::is_integral_v<T>) {
      return RGB_base<T>(
        std::clamp<int>(int(R) * other, 0, max_val),
        std::clamp<int>(int(G) * other, 0, max_val),
        std::clamp<int>(int(B) * other, 0, max_val)
      );
    } else if (std::is_floating_point_v<T>) {
      return RGB_base<T>(
        std::clamp<double>(double(R) * other, 0, max_val),
        std::clamp<double>(double(G) * other, 0, max_val),
        std::clamp<double>(double(B) * other, 0, max_val)
      );
    }
  }
  
  template <typename T2>
  void operator ^= (CN(RGB_base<T2>) other) {
    R = int(R) ^ int(other.R);
    G = int(G) ^ int(other.G);
    B = int(B) ^ int(other.B);
  }

  template <typename T2>
  void operator += (CN(RGB_base<T2>) other) {
    if constexpr (std::is_integral_v<T>) {
      R = std::clamp<int>(int(R) + other.R, 0, max_val);
      G = std::clamp<int>(int(G) + other.G, 0, max_val);
      B = std::clamp<int>(int(B) + other.B, 0, max_val);
    } else if (std::is_floating_point_v<T>) {
      R = std::clamp<double>(double(R) + other.R, 0, max_val);
      G = std::clamp<double>(double(G) + other.G, 0, max_val);
      B = std::clamp<double>(double(B) + other.B, 0, max_val);
    }
  }

  template <typename T2>
  void operator += (T2 other) {
    if constexpr (std::is_integral_v<T>) {
      R = std::clamp<int>(int(R) + other, 0, max_val);
      G = std::clamp<int>(int(G) + other, 0, max_val);
      B = std::clamp<int>(int(B) + other, 0, max_val);
    } else if (std::is_floating_point_v<T>) {
      R = std::clamp<double>(double(R) + other, 0, max_val);
      G = std::clamp<double>(double(G) + other, 0, max_val);
      B = std::clamp<double>(double(B) + other, 0, max_val);
    }
  }

  template <typename T2>
  void operator /= (CN(RGB_base<T2>) other) {
    if constexpr (std::is_integral_v<T>) {
      R = std::clamp<int>(int(R) / other.R, 0, max_val);
      G = std::clamp<int>(int(G) / other.G, 0, max_val);
      B = std::clamp<int>(int(B) / other.B, 0, max_val);
    } else if (std::is_floating_point_v<T>) {
      R = std::clamp<double>(double(R) / other.R, 0, max_val);
      G = std::clamp<double>(double(G) / other.G, 0, max_val);
      B = std::clamp<double>(double(B) / other.B, 0, max_val);
    }
  }
}; // RGB24

using RGB24 = RGB_base<byte>;
using RGBi = RGB_base<int>;
using RGBf = RGB_base<float, 1>;
using RGBld = RGB_base<ldouble, 1>;

} // seze ns
