#include <cmath>
#include "diff.hpp"

seze::RGB24 Color_selector_diff::operator()(CN(seze::RGB24) src,
CN(seze::RGB24) a, CN(seze::RGB24) b) {
  auto diff_a {
    std::abs(src.R - a.R) +
    std::abs(src.G - a.G) +
    std::abs(src.B - a.B)
  };
  auto diff_b {
    std::abs(src.R - b.R) +
    std::abs(src.G - b.G) +
    std::abs(src.B - b.B)
  };
  if (diff_a <= diff_b)
    return a;
  return b;
}
