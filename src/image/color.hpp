#pragma once
#include "../utils/macro.hpp"
extern "C" {
#include "../plugins/color-type.h"
}

namespace seze {

//! universal pixel data maker
byte* make_pixels(color_t type, int pixel_count);
void destroy_pixels(byte* data);
size_t get_size(color_t type);

} // seze ns
