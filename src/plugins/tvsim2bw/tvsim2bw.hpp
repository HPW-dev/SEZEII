#pragma once
#include <mutex>
#include "utils/macro.hpp"
#include "types.hpp"

namespace seze {
  class Image;
}

class Tvsim2bw final {
  nocopy(Tvsim2bw);

  std::once_flag once_flag {};
  shared_p<seze::Image> bw_img {};

public:
  desaturation_e desat_type {desaturation_e::average};

  Tvsim2bw() = default;
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst);
}; // Tvsim2bw
