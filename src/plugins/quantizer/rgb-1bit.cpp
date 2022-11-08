#include "rgb-1bit.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

//! fast r g b precalculated table
static seze::RGB24 table[2][2][2] {};

RGB_1bit_qnt::RGB_1bit_qnt() {
  // init table
  FOR (ir, 2)
    FOR (ig, 2)
      FOR (ib, 2)
        table[ir][ig][ib] = seze::RGB24(ir*255, ig*255, ib*255);
}

void RGB_1bit_qnt::operator()(seze::Image &dst) {
  FOR (i, dst.size) {
    auto &color {dst.fast_get<seze::RGB24>(i)};
    // (0..255) / 128 = (0..1) idx for table
    const auto ir {color.R >> 7};
    const auto ig {color.G >> 7};
    const auto ib {color.B >> 7};
    color = table[ir][ig][ib];
  }
}
