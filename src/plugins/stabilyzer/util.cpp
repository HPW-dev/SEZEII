#include "util.hpp"
//#include "utils/log.hpp"

bool check_color(CN(seze::RGB24) a, CN(seze::RGB24) b) {
  auto diff = std::abs(int(a.R) - b.R);
  if (diff > threshold_r)
    return false;
  diff = std::abs(int(a.G) - b.G);
  if (diff > threshold_g)
    return false;
  diff = std::abs(int(a.B) - b.B);
  if (diff > threshold_b)
    return false;
  return true;
}

void find_point(CN(seze::Image) src, Point& dst,
CN(seze::RGB24) color) {
  FOR (y, src.Y)
  FOR (x, src.X)
    if (check_color(src.fast_get<seze::RGB24>(x, y), color)) {
      dst.x = x;
      dst.y = y;
      return;
    }
} // find_point

void find_points(CN(seze::Image) src) {
  find_point(src, pos_a, color_a);
  find_point(src, pos_b, color_b);
  find_point(src, pos_c, color_c);
  find_point(src, pos_d, color_d);
  /*LOG("a.x:" << pos_a.x << " a.y:" << pos_a.y <<
    " b.x:" << pos_b.x << " b.y:" << pos_b.y <<
    " c.x:" << pos_c.x << " c.y:" << pos_c.y <<
    " d.x:" << pos_d.x << " d.y:" << pos_d.y << "\n");*/
}

Point calc_center(CN(seze::Image) dst, CN(seze::Image) src)
  { return {.x = dst.X / 2 - src.X / 2, .y = dst.Y / 2 - src.Y / 2}; }
