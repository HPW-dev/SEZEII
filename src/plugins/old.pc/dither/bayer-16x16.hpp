#pragma once
#include "dither.hpp"

class Dither_bayer_16x16: public Dither {
  Real matrix[16][16];

public:
  Dither_bayer_16x16();
  void operator()(seze::Image &dst,
    CN(urect) area, CN(Palette) pal,
    Palette_accepter *palette_accepter, Real power=1) override;
};
