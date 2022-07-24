#pragma once
#include "utils/vec2.hpp"
#include "utils/macro.hpp"
#include "palette.hpp"
#include "utils/rect.hpp"

namespace seze { class Image; }
class Color_finder;
class Color_selector;
class Palette_accepter;

class Old_pc {
  Palette pal {};
  vec2u block_size {};
  Color_finder *color_finder {};
  Color_selector *color_selector {};
  Palette_accepter *palette_accepter {};

  void process_block(seze::Image &dst, CN(urect) area);

public:
  Old_pc(CN(Palette) pal_, CN(vec2u) block_size_,
    Color_finder *color_finder_, Color_selector *color_selector_,
    Palette_accepter *palette_accepter_);
  void operator()(seze::Image &dst);
};
