#include "image.hpp"
#include <cstring>

namespace seze {

Image::Image(int x, int y, color_t color_type)
: X_(x)
, Y_(y)
, stride_(0)
, size_(x * y)
, bytes_(0)
, type_(color_type)
, data_(make_pixels(color_type, x * y))
{ 
  stride_ = data_ ? get_size(color_type) * x : 0;
  bytes_ = stride_ * y;
}

void Image::init(int x, int y, color_t color_type) {
  destroy_pixels(data_);
  X_ = x;
  Y_ = y;
  size_ = x * y;
  type_ = color_type;
  data_ = make_pixels(color_type, x * y);
  stride_ = data_ ? get_size(color_type) * x : 0;
  bytes_ = stride_ * y;
}

Image::Image(byte* _data_, int x, int y, color_t color_type)
: X_(x)
, Y_(y)
, stride_(0)
, size_(x * y)
, bytes_(0)
, type_(color_type)
, data_(_data_)
, no_destroy(true)
{ 
  stride_ = data_ ? get_size(color_type) * x : 0;
  bytes_ = stride_ * y;
}

Image::Image(CN(Image) src)
: X_(src.X)
, Y_(src.Y)
, stride_(src.stride)
, size_(src.size)
, bytes_(src.bytes)
, type_(src.type)
{ 
  data_ = make_pixels(type_, size_);
  memcpy(data_, src.get_cdata(), bytes_);
}

Image::~Image() {
  if ( !no_destroy)
    destroy_pixels(data_);
}

void Image::fast_copy_to(Image& dst) const
  { memcpy(dst.data_, data_, bytes_); }

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
      return true;
    }
  } // switch (mode)
  return false;
} // prepare_cord

byte* Image::get_data() const { return data_; }
CP(byte) Image::get_cdata() const { return data_; }

} // seze ns
