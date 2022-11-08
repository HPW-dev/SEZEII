#include "color.hpp"
#include "utils/error.hpp"
#ifndef NOAALLOC
#include "utils/aalloc.hpp"
#endif
#include "RGB555-RGB565.hpp"
#include "rgb24.hpp"
#include "YUV.hpp"
#include <map>

namespace seze {

template <class T>
byte* _make_pixels(size_t size) {
#ifdef NOAALLOC
  return rcast(byte*, new T[size]);
#else
  return rcast(byte*, aalloc(ALIGN, size * sizeof(T)));
#endif
}

byte* make_pixels(color_t type, int pixel_count) {
  auto size = pixel_count * get_size(type);
  switch (type) {
    case seze_RGB555: return _make_pixels<RGB555>(size); break;
    case seze_RGB565: return _make_pixels<RGB565>(size); break;
    case seze_YUVf: return _make_pixels<YUVf>(size); break;
    case seze_YUVld: return _make_pixels<YUVld>(size); break;
    case seze_YUV24: return _make_pixels<YUV24>(size); break;
    case seze_RAW: return _make_pixels<byte>(size); break;
    case seze_RGB24: return _make_pixels<RGB24>(size); break;
    case seze_f_gray: return _make_pixels<double>(size); break;
    case seze_ld_gray: return _make_pixels<long double>(size); break;
    case seze_gray: return _make_pixels<byte>(size); break;
    case seze_RGBi: return _make_pixels<RGBi>(size); break;
    case seze_RGBf: return _make_pixels<RGBf>(size); break;
    case seze_RGBld: return _make_pixels<RGBld>(size); break;
    case seze_none: error("make_pixels: none pix fmt"); break;
    default: {
      error("make_pixels: unknown pixel format. need impl.");
      return nullptr;
    }
  }
  return nullptr;
} // make_pixels

void destroy_pixels(byte* data) {
#ifdef NOAALLOC
  delete[] data;
#else
  afree(data);
#endif
}

size_t get_size(color_t type) {
  std::map<color_t, size_t> table = {
    {seze_none, 0},
    {seze_RAW, 1},
    {seze_RGB8, 1},
    {seze_RGB24, 3},
    {seze_BGR24, 3},
    {seze_RGB555, 2},
    {seze_RGB565, 2},
    {seze_YUV24, 3},
    {seze_RGBi, 3 * sizeof(int)},
    {seze_RGBf, 3 * sizeof(float)},
    {seze_RGBld, 3 * sizeof(ldouble)},
    {seze_YUVf, sizeof(float) * 3},
    {seze_YUVld, sizeof(ldouble) * 3},
    {seze_f_gray, sizeof(float)},
    {seze_ld_gray, sizeof(ldouble)},
    {seze_gray, 1},
  };
  try {
    return table.at(type);
  } catch (...) {
    error("get_size: mapping error");
  }
  return 0;
} // get_size

} // seze ns
