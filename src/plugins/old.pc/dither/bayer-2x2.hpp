#pragma once
#include "dither.hpp"

class Dither_bayer_2x2: public Dither {
public:
  void operator()(seze::Image &dst,
    CN(urect) area, CN(Palette) pal,
    Palette_accepter *palette_accepter, Real power=1) override;
};
