#pragma once

#ifndef COLOR_T
#define COLOR_T
enum color_t {
  seze_none,
  seze_RAW, /// direct topy
  seze_RGB8, ///< 3:3:2
  seze_RGB24,
  seze_BGR24,
  seze_RGB555,
  seze_RGB565,
  seze_YUV24,
  seze_YUVf,
  seze_YUVld,
  seze_RGBi,
  seze_RGBf,
  seze_RGBld,
  seze_f_gray, ///< float gray
  seze_ld_gray, ///< long double gray
  seze_gray, ///< byte gray
};
#endif
