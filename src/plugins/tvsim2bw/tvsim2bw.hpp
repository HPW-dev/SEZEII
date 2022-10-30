#pragma once
#include "utils/macro.hpp"
#include "utils/types.hpp"

namespace seze {
  class Image;
}

class Tvsim2bw final {
  nocopy(Tvsim2bw);

public:
  

  Tvsim2bw() = default;
  ~Tvsim2bw() = default;
  //! main processing func
  void operator ()(CN(seze::Image) src, seze::Image &dst);
};
