#pragma once
#include "../../image/image.hpp"
#include "../../utils/error.hpp"
#include "config.hpp"

///! tv sim's base class
class TVsim {
public:
  virtual void process(seze::Image& dst) = 0;
  virtual ~TVsim() = default;
  void check_config();
};
