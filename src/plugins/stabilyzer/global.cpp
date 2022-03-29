#include "global.hpp"
#include "math.hpp"
#include "Image/Image.hpp"

bool first_init_flag = false;
bool abcd_mode = false;
seze::Image *big_buffer = {};
seze::RGB24 color_a(0, 203, 255);
seze::RGB24 color_b(208, 0, 255);
seze::RGB24 color_c(255, 233, 0);
seze::RGB24 color_d(0, 255, 0);
byte threshold_r = 10;
byte threshold_g = 10;
byte threshold_b = 10;
Point pos_a = {.x=0, .y=0};
Point pos_b = {.x=20, .y=0};
Point pos_c = {.x=0, .y=20};
Point pos_d = {.x=20, .y=20};
rotate_ft rotate_f = {};
stretch_ft stretch_f = {};
resize_ft resize_f = {};
