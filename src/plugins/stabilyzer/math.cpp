#include <cmath>
#include "math.hpp"

Real radian_btw_point(CN(Point) a, CN(Point) b) {
  Point diff{.x=b.x - a.x, .y=b.y - a.y};
  Real len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
  if (len <= 0)
    return 0;
  len = 1.0 / len; // оптимизация деления
  return std::atan2(diff.x * len, diff.y * len);
}

void rotate_point(CN(Point) center, Point& dst, Real radian) {
  Real mul_x = std::cos(radian);
  Real mul_y = std::sin(radian);
  dst.x = mul_x * (dst.x - center.x) - mul_y * (dst.x - center.y) + center.x;
  dst.y = mul_y * (dst.y - center.x) + mul_x * (dst.y - center.y) + center.y;
}
