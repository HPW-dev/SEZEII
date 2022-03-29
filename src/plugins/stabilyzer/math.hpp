#pragma once
#include <numbers>
#include "utils/macro.hpp"
#include "utils/types.hpp"

struct Point { 
  int x=0, y=0;

  template <typename T>
  constexpr Point operator * (CN(T) val)
    { return {.x = x * val, .y = y * val}; }
  
  constexpr void operator -= (CN(Point) other) {
    x -= other.x;
    y -= other.y;
  }
}; // Point

//! узнаеёт угол в радианах между точкой a и b
Real radian_btw_point(CN(Point) a, CN(Point) b);
void rotate_point(CN(Point) center, Point& dst, Real radian);

constexpr Real to_radian(Real degree) { 
  constexpr Real mul = std::numbers::pi / 180.0;
  return degree * mul;
}
