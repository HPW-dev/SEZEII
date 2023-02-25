#include "rgb242.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

void RGB242_qnt::operator()(seze::Image &dst) {
  cfor (i, dst.size) {
    auto &color {dst.fast_get<seze::RGB24>(i)};
    constexpr int fmul_r {255 / 3};
    constexpr int fmul_g {255 / 15};
    constexpr int fmul_b {fmul_r};
    const int r (int(color.R >> 6) * fmul_r);
    const int g (int(color.G >> 4) * fmul_g);
    const int b (int(color.B >> 6) * fmul_b);
    color = seze::RGB24(r, g, b);
  }
}
