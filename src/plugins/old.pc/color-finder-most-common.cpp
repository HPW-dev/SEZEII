#include <vector>
#include <algorithm>
#include "color-finder-most-common.hpp"
#include "image/image.hpp"

struct Color_count {
  seze::RGB24 col {};
  uint cnt {0};
};

static bool operator ==(CN(Color_count) src, CN(seze::RGB24) col)
  { return src.col == col; }

void Color_finder_most_common::operator()(CN(seze::Image) src, CN(urect) area,
seze::RGB24 &a, seze::RGB24 &b) {
  /* создать список самых часто встречаемых цветов и
  выбрать из них последний и первый цвет в списке*/
  std::vector<Color_count> most_common;
  for (uint y {area.pos.y}; y < area.pos.y + area.size.y; ++y)
  for (uint x {area.pos.x}; x < area.pos.x + area.size.x; ++x) {
    auto col_src {src.fast_get<seze::RGB24>(x, y)};
    auto it {std::find(most_common.begin(), most_common.end(), col_src)};
    if (it == most_common.end())
      most_common.push_back({col_src, 0});
    else
      ++it->cnt;
  }
  // отсортировать по ворзастанию
  std::sort(most_common.begin(), most_common.end(), 
    [](CN(Color_count) a, CN(Color_count) b)->bool
      { return a.cnt < b.cnt; }
  );
  // если список из одного элемента:
  b = most_common.back().col;
  if (most_common.size() > 1)
    a = most_common.front().col;
  else
    a = b;
} // ()
