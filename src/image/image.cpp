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
  data = new byte[BYTES];
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

} // seze ns
