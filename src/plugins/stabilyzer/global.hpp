#pragma once
#include <vector>
#include "Image/rgb24.hpp"
#include "utils/macro.hpp"
#include "utils/types.hpp"

namespace seze {
  class Image;
}
struct Point;
using v_pixel_t = std::vector<seze::RGB24>;
using rotate_ft = void(*)(CN(seze::Image), seze::Image&, CN(Point), Real);
using stretch_ft = void(*)(CN(v_pixel_t), v_pixel_t&);
using resize_ft = void(*)(CN(seze::Image), seze::Image&, CN(Point), int, int);

extern bool first_init_flag;
extern bool abcd_mode;
extern seze::Image *big_buffer;
extern seze::RGB24 color_a;
extern seze::RGB24 color_b;
extern seze::RGB24 color_c;
extern seze::RGB24 color_d;
extern byte threshold_r;
extern byte threshold_g;
extern byte threshold_b;
extern Point pos_a;
extern Point pos_b;
extern Point pos_c;
extern Point pos_d;
extern rotate_ft rotate_f;
extern stretch_ft stretch_f;
extern resize_ft resize_f;
