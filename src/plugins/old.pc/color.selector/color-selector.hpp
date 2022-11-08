#pragma once
#include "image/rgb24.hpp"

//! функтор, который сравнивает цвета и подбирает подходящий из палитры 
class Color_selector {
public:
  Color_selector() = default;
  virtual ~Color_selector() = default;
  virtual seze::RGB24 operator()(CN(seze::RGB24) src,
    CN(seze::RGB24) a, CN(seze::RGB24) b) = 0;
};
