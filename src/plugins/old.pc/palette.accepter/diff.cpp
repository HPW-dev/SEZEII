#include <cmath>
#include "diff.hpp"

seze::RGB24 Palette_accepter_diff::operator()
(CN(seze::RGB24) src, CN(Palette) pal) {
  seze::RGB24 ret {pal.front()};
  // выбрать цвет с палитры, у которого самая маленькая разница между src
  int ret_diff {255 + 255 + 255};
  for (CN(auto) pal_col: pal) {
    auto diff {
      std::abs(src.R - pal_col.R) +
      std::abs(src.G - pal_col.G) +
      std::abs(src.B - pal_col.B)
    };
    if (ret_diff > diff) {
      ret_diff = diff;
      ret = pal_col;
    }
  }
  return ret;
}
