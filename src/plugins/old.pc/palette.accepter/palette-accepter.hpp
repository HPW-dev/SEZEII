#pragma once
#include "image/rgb24.hpp"
#include "old.pc/palette.hpp"

//! функтор для конвертирования цвета в палитровый цвет
class Palette_accepter {
public:
  Palette_accepter() = default;
  virtual ~Palette_accepter() = default;
  virtual seze::RGB24 operator ()(CN(seze::RGB24) src, CN(Palette) pal) = 0;
};
