#pragma once
#include "color.hpp"

namespace seze {
  
class Image {
  int X = 0, Y = 0, STRIDE = 0, SIZE = 0;
  int BYTES = 0; ///< total bytes for pixel data
  color_t TYPE = color_t::none; ///< pixel color type
  byte* data = nullptr; ///< pixel data
  //! if maked from input mem, not use free
  bool no_destroy = false;
public:
  Image() = default;
  Image(CN(Image) src);
  //! init by size & pixel type
  Image(int x, int y, color_t color_type);
  //! init by prepeared data
  Image(byte* data_, int x, int y, color_t color_type);
  ~Image();
  constexpr byte* get_data() const { return data; }
  constexpr CP(byte) get_cdata() const { return data; }
  constexpr CN(int) get_x() const { return X; }
  constexpr CN(int) get_y() const { return Y; }
  CN(int) get_stride() const;
  CN(int) size() const;
  constexpr CN(int) bytes() const { return BYTES; }
  CN(color_t) type() const;
  void fast_copy_to(Image& dst) const;

  template<class T> T& fast_get(int i) const {
    auto p = rcast(T*, data);
    return p[i];
  }
  template<class T> T& fast_get(int x, int y) const {
    auto p = rcast(T*, data);
    return p[X * y + x];
  }
  template<class T> void fast_set(int i, CN(T) c) const {
    auto p = rcast(T*, data);
    p[i] = c;
  }
  template<class T> void fast_set(int x, int y, CN(T) c) const {
    auto p = rcast(T*, data);
    p[X * y + x] = c;
  }
  template<class T> T get(int x, int y) const {
    if (uint(x) >= uint(X) or uint(y) >= uint(Y))
      return {};
    auto p = rcast(T*, data);
    return p[X * y + x];
  }
  template<class T> T* get_ptr(int x, int y) const {
    if (uint(x) >= uint(X) or uint(y) >= uint(Y))
      return nullptr;
    auto p = rcast(T*, data);
    return &p[X * y + x];
  }
  template<class T> void set(int x, int y, CN(T) c) const {
    if (uint(x) >= uint(X) or uint(y) >= uint(Y))
      return;
    auto p = rcast(T*, data);
    p[X * y + x] = c;
  }
}; // Image

} // seze ns
