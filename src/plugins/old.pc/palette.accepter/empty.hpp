#pragma once
#include "palette-accepter.hpp"

class Palette_accepter_empty: public Palette_accepter {
public:
  Palette_accepter_empty() = default;
  inline seze::RGB24 operator ()(CN(seze::RGB24) src, CN(Palette) pal) override
    { return src; }
};
