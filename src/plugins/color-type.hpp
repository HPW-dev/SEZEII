#pragma once

namespace seze {

enum class color_t {
  none=0,
  RAW, /// direct topy
  RGB8, ///< 3:3:2
  RGB24, BGR24,
  RGB555, RGB565,
  YUV24, YUVf, YUVld,
  RGBi, RGBf, RGBld,
  f_gray, ///< float gray
  ld_gray, ///< long double gray
  gray, ///< byte gray
};

} // seze ns
