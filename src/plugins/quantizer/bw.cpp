#include <array>
#include "bw.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

void BW_qnt::operator()(seze::Image &dst) {
  constexpr static const seze::RGB24 table[2] {
    seze::RGB24(0,0,0),
    seze::RGB24(255,255,255),
  };
  FOR (i, dst.size) {
    auto &color {dst.fast_get<seze::RGB24>(i)};
    // average / 128 == 0 or 1 -> index in table
    const auto id {((color.R + color.G + color.B) / 3) >> 7};
    color = table[id];
  }
} // ()
