#pragma once

namespace seze {
  class Image;
}

//! база для палитровых квантователей
class Quantizer {
public:
  virtual void operator()(seze::Image &dst) = 0;
  virtual ~Quantizer() = default;
};
