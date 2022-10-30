#pragma once
#include "utils/types.hpp"

using luma_t = real;

enum class desaturation_e {
  average = 0,
  bt2001,
  bt709,
  bt601,
  hsl,
  euclidian_distance,
  red,
  green,
  blue,

  _max_,
};
