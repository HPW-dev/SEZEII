#pragma once
#include "color-finder.hpp"

class Color_finder_minmax: public Color_finder {
public:
  Color_finder_minmax() = default;
  void operator()(CN(seze::Image) src, CN(urect) area,
    seze::RGB24 &a, seze::RGB24 &b) override;
};
