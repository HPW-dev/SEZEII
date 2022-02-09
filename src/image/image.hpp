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
  int X_ = 0, Y_ = 0, stride_ = 0, size_ = 0;
  int bytes_ = 0; ///< total bytes for pixel data
  color_t type_ = seze_none; ///< pixel color type
  byte* data_ = nullptr; ///< pixel data
  //! if maked from input mem, not use free
  bool no_destroy = false;

  //! see boundig_e desc.
  bool prepare_cord(int& x, int& y, boundig_e mode) const;

public:
  CN(int) X = X_;
  CN(int) Y = Y_;
  CN(int) stride = stride_;
  CN(int) size = size_;
  CN(int) bytes = bytes_;
  CN(color_t) type = type_;

  Image() = default;
  Image(CN(Image) src);
  //! init by size & pixel type
  Image(int x, int y, color_t color_type);
  //! init by prepeared data
  Image(byte* data_, int x, int y, color_t color_type);
  ~Image();
  byte* get_data() const;
  CP(byte) get_cdata() const;
  void fast_copy_to(Image& dst) const;
  // template members
  #include "image.inc"
}; // Image



} // seze ns
