#pragma once
#include "quantizer.hpp"

//! 2 bit for all rgb component
class RGB_2bit_qnt: public Quantizer {
public:
  void operator()(seze::Image &dst) override;
  RGB_2bit_qnt();
  ~RGB_2bit_qnt() = default;
};
