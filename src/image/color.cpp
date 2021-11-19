#include "color.hpp"
#include "../utils/error.hpp"
#ifndef NOAALLOC
#include "../utils/aalloc.hpp"
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
  // TODO aligned alloc
  switch (type) {
    case color_t::RGB555: return _make_pixels<RGB555>(size); break;
    case color_t::RGB565: return _make_pixels<RGB565>(size); break;
    case color_t::YUVf: return _make_pixels<YUVf>(size); break;
    case color_t::YUVld: return _make_pixels<YUVld>(size); break;
    case color_t::YUV24: return _make_pixels<YUV24>(size); break;
    case color_t::RAW: return _make_pixels<byte>(size); break;
    case color_t::RGB24: return _make_pixels<RGB24>(size); break;
    case color_t::f_gray: return _make_pixels<double>(size); break;
    case color_t::ld_gray: return _make_pixels<long double>(size); break;
    case color_t::gray: return _make_pixels<byte>(size); break;
    case color_t::RGBi: return _make_pixels<RGBi>(size); break;
    case color_t::RGBf: return _make_pixels<RGBf>(size); break;
    case color_t::RGBld: return _make_pixels<RGBld>(size); break;
    case color_t::none: error("make_pixels: none pix fmt"); break;
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
    {color_t::none, 0},
    {color_t::RAW, 1},
    {color_t::RGB8, 1},
    {color_t::RGB24, 3},
    {color_t::BGR24, 3},
    {color_t::RGB555, 2},
    {color_t::RGB565, 2},
    {color_t::YUV24, 3},
    {color_t::RGBi, 3 * sizeof(int)},
    {color_t::RGBf, 3 * sizeof(float)},
    {color_t::RGBld, 3 * sizeof(ldouble)},
    {color_t::YUVf, sizeof(float) * 3},
    {color_t::YUVld, sizeof(ldouble) * 3},
    {color_t::f_gray, sizeof(float)},
    {color_t::ld_gray, sizeof(ldouble)},
    {color_t::gray, 1},
  };
  try {
    return table.at(type);
  } catch (...) {
    error("get_size: mapping error");
  }
} // get_size

} // seze ns
