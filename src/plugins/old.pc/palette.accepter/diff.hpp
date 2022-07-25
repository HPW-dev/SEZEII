#pragma once
#include "palette-accepter.hpp"

class Palette_accepter_diff: public Palette_accepter {
public:
  Palette_accepter_diff() = default;
  seze::RGB24 operator ()(CN(seze::RGB24) src, CN(Palette) pal) override;
};
