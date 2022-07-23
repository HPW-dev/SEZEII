#pragma once
#include "quantizer.hpp"

//! 1 bit for all rgb component
class RGB_1bit_qnt: public Quantizer {
public:
  void operator()(seze::Image &dst) override;
  RGB_1bit_qnt();
  ~RGB_1bit_qnt() = default;
};
