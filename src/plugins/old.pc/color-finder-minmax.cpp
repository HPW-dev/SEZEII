#include "color-finder-minmax.hpp"
#include "image/image.hpp"

void Color_finder_minmax::operator()(CN(seze::Image) src, CN(urect) area,
seze::RGB24 &a, seze::RGB24 &b) {
  seze::RGB24 col_min {};
  seze::RGB24 col_max {};
  uint col_max_sum {};
  uint col_min_sum {};
  col_max_sum = 0;
  col_min_sum = 255 + 255 + 255;
  col_max = col_min = {};
  // найти два цвета для блока
  for (uint y = area.pos.y; y < area.pos.y + area.size.y; ++y)
  for (uint x = area.pos.x; x < area.pos.x + area.size.x; ++x) {
    auto col_src {src.fast_get<seze::RGB24>(x, y)};
    auto sum {col_src.R + col_src.G + col_src.B};
    if (sum > col_max_sum) {
      col_max_sum = sum;
      col_max = col_src;
    }
    if (sum < col_min_sum) {
      col_min_sum = sum;
      col_min = col_src;
    }
  }
  a = col_min;
  b = col_max;
} // ()
