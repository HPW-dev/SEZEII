#include "rgb-2bit.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

//! fast r g b precalculated table
static seze::RGB24 table[4][4][4] {};

RGB_2bit_qnt::RGB_2bit_qnt() {
  constexpr float alpha {255.0f / 3.0f};
  // init table
  FOR (ir, 4)
    FOR (ig, 4)
      FOR (ib, 4)
        table[ir][ig][ib] = seze::RGB24(ir*alpha, ig*alpha, ib*alpha);
}

void RGB_2bit_qnt::operator()(seze::Image &dst) {
  FOR (i, dst.size) {
    auto &color {dst.fast_get<seze::RGB24>(i)};
    const auto ir {color.R >> 6};
    const auto ig {color.G >> 6};
    const auto ib {color.B >> 6};
    color = table[ir][ig][ib];
  }
}
