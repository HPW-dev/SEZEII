#pragma once
#include "src/plugins/old.pc/palette.hpp"
#include "utils/types.hpp"
#include "utils/rect.hpp"

class Palette_accepter;
namespace seze { class Image; }

class Dither {
public:
  virtual ~Dither() = default;
  virtual void operator()(seze::Image &dst,
    CN(urect) area, CN(Palette) pal,
    Palette_accepter *palette_accepter, Real power=1) = 0;
};
