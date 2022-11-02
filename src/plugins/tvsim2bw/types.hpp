#pragma once
#include "utils/types.hpp"

using luma_t = real; //! юзать для чтения с картинки формата seze_f_gray
using v_luma_t = vector_t<luma_t>;

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
}; // desaturation_e

enum class scale_e {
  nearest = 0,
  bilinear,
  bicubic,
  bilinear_fast,
  bicubic_fast,
}; // scale_e

enum class filter_e {
  none = 0,
  average,
  average_fast,
  median,
};
