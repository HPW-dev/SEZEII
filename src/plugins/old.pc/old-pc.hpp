#pragma once
#include "utils/vec2.hpp"
#include "utils/macro.hpp"
#include "palette.hpp"
#include "utils/rect.hpp"
#include "utils/types.hpp"

namespace seze { class Image; }
class Color_finder;
class Color_selector;
class Palette_accepter;
class Dither;

class Old_pc {
  Palette pal {};
  vec2u block_size {};
  Color_finder *color_finder {};
  Color_selector *color_selector {};
  Palette_accepter *palette_accepter {};
  Dither *dither {};
  Real dither_power {1};

  void process_block(seze::Image &dst, CN(urect) area);
  void check() const;

public:
  Old_pc() = default;
  void set_pal(CN(Palette) pal_);
  void set_block_size(CN(vec2u) block_size_);
  void set_color_finder(Color_finder* color_finder_);
  void set_color_selector(Color_selector* color_selector_);
  void set_palette_accepter(Palette_accepter* palette_accepter_);
  void set_dither(Dither* dither_, Real dither_power_=1);
  void operator()(seze::Image &dst);
}; // Old_pc
