#include "image.hpp"
#include <cstring>

namespace seze {

Image::Image(int x, int y, color_t color_type)
: X(x)
, Y(y)
, STRIDE(0)
, SIZE(x * y)
, BYTES(0)
, TYPE(color_type)
, data(make_pixels(color_type, x * y))
{ 
  STRIDE = data ? get_size(color_type) * x : 0;
  BYTES = STRIDE * y;
}

Image::Image(byte* data_, int x, int y, color_t color_type)
: X(x)
, Y(y)
, STRIDE(0)
, SIZE(x * y)
, BYTES(0)
, TYPE(color_type)
, data(data_)
, no_destroy(true)
{ 
  STRIDE = data ? get_size(color_type) * x : 0;
  BYTES = STRIDE * y;
}

Image::Image(CN(Image) src)
: X(src.get_x())
, Y(src.get_y())
, STRIDE(src.get_stride())
, SIZE(src.size())
, BYTES(src.bytes())
, TYPE(src.type())
{ 
  data = make_pixels(TYPE, SIZE);
  memcpy(data, src.get_cdata(), BYTES);
}

Image::~Image() {
  if ( !no_destroy)
    destroy_pixels(data);
}

CN(color_t) Image::type() const { return TYPE; }
CN(int) Image::get_stride() const { return STRIDE; }
CN(int) Image::size() const { return SIZE; }
void Image::fast_copy_to(Image& dst) const
  { memcpy(dst.get_data(), data, BYTES); }

bool Image::prepare_cord(int& x, int& y, boundig_e mode) const {
  switch (mode) {
    case boundig_e::mirror: {
      while (x < 0) x = X - (X + x);
      while (x >= X) x = X - 1 + ((x - X) * -1);
      while (y < 0) y = Y - (Y + y);
      while (y >= Y) y = Y - 1 + ((y - Y) * -1);
      return true;
    }
    case boundig_e::clamp: {
      x = std::clamp(x, 0, X - 1);
      y = std::clamp(y, 0, Y - 1);
      return true;
    }
    case boundig_e::none:
    default: {
      if (uint(x) >= uint(X) or uint(y) >= uint(Y))
        return false;
      return false;
    }
  } // switch (mode)
  return false;
} // prepare_cord

} // seze ns
