#pragma once
#include "image/rgb24.hpp"
#include "utils/rect.hpp"

namespace seze { class Image; }

//! база для функционала поиска двух цветов для блока
class Color_finder {
public:
  virtual ~Color_finder() = default;
  virtual void operator()(CN(seze::Image) src, CN(urect) area,
    seze::RGB24 &a, seze::RGB24 &b) = 0;
};
