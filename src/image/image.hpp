#pragma once
#include "color.hpp"
#include <algorithm>

namespace seze {

//! что делать, если пиксель берётся за пределами картинки
enum boundig_e {
  none, ///< чёрный пиксель
  mirror, ///< зеркальная копия
  clamp, ///< не выходить за рамки
};

class Image {
  int X = 0, Y = 0, STRIDE = 0, SIZE = 0;
  int BYTES = 0; ///< total bytes for pixel data
  color_t TYPE = seze_none; ///< pixel color type
  byte* data = nullptr; ///< pixel data
  //! if maked from input mem, not use free
  bool no_destroy = false;

  //! see boundig_e desc.
  bool prepare_cord(int& x, int& y, boundig_e mode) const;

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
  // template members
  #include "image.inc"
}; // Image



} // seze ns
