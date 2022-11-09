#pragma once
#include "conf.hpp"

namespace seze {
  class Image;
}

class Tvsim2_base {
  nocopy(Tvsim2_base);

public:
  tvsim_conf conf {};
  tvsim_conf_yuv conf_yuv {};
  Str title {};

  Tvsim2_base() = default;
  virtual ~Tvsim2_base() = default;
  //! main processing func
  virtual void operator ()(CN(seze::Image) src, seze::Image &dst) = 0;
}; // Tvsim2_base
