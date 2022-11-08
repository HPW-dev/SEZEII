#pragma once
#include "tvsim2bw.hpp"

class Tvsim2yuv: public Tvsim2bw {
  nocopy(Tvsim2yuv);

public:
  Tvsim2yuv();
  ~Tvsim2yuv() = default;
  //! main processing func
  //void operator ()(CN(seze::Image) src, seze::Image &dst) override;
}; // Tvsim2bw
