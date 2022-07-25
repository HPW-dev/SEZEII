#pragma once
#include "color-selector.hpp"

//! поиск цвета с по разнице в цветах
class Color_selector_diff: public Color_selector {
public:
  Color_selector_diff() = default;
  seze::RGB24 operator()(CN(seze::RGB24) src,
    CN(seze::RGB24) a, CN(seze::RGB24) b) override;
};
