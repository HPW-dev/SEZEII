#pragma once
#include "utils/macro.hpp"
#include "utils/point.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

class Tvsim2bw final {
  nocopy(Tvsim2bw);

  shared_p<seze::Image> bw_img {};
  shared_p<seze::Image> bw_img_scaled {};

  void downscale();
  void upscale();
  void desaturate_(CN(seze::Image) src);
  void convert_to_dst(seze::Image &dst);

public:
  desaturation_e desat_type {desaturation_e::average};
  scale_e scale_type_in {scale_e::bilinear};
  scale_e scale_type_out {scale_e::bilinear};
  Point<int> scale_wh {320, 240};
  bool use_scale {true}; ///< при 0 юзает RAW разрешение

  Tvsim2bw();
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst);
}; // Tvsim2bw
