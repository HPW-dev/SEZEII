#pragma once
#include "utils/macro.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

void gray_to_rgb24(CN(seze::Image) src, seze::Image &dst);
void desaturate(CN(seze::Image) src, seze::Image &dst,
  desaturation_e type);
void scale_gray(CN(seze::Image) src, seze::Image &dst,
  scale_e type = scale_e::bilinear);
