#pragma once
#include "quantizer.hpp"

//! RRGGGGBB
class RGB242_qnt: public Quantizer {
public:
  void operator()(seze::Image &dst) override;
  ~RGB242_qnt() = default;
};
