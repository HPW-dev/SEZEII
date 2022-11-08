#pragma once
#include "quantizer.hpp"

//! чёрный и белый цвет
class BW_qnt: public Quantizer {
public:
  void operator()(seze::Image &dst) override;
  ~BW_qnt() = default;
};
