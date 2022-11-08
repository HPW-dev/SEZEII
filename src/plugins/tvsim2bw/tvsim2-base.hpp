#pragma once
#include "conf.hpp"

namespace seze {
  class Image;
}

class Tvsim2_base {
  nocopy(Tvsim2_base);

public:
  tvsim_conf conf {};

  Tvsim2_base() = default;
  virtual ~Tvsim2_base() = default;
  //! main processing func
  virtual void operator ()(CN(seze::Image) src, seze::Image &dst) = 0;
}; // Tvsim2_base
