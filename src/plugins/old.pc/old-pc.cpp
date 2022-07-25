#include "old-pc.hpp"
#include "image/image.hpp"
#include "utils/error.hpp"
#include "color.finder/color-finder.hpp"
#include "color.selector/color-selector.hpp"
#include "palette.accepter/palette-accepter.hpp"
#include "dither/dither.hpp"

void Old_pc::operator()(seze::Image &dst) {
  // поделить картинку на блоки и работать только с ними
  uint ix_max {dst.X / block_size.x};
  iferror (ix_max == 0, "Old_pc(): ix_max == 0");
  uint iy_max {dst.Y / block_size.y};
  iferror (iy_max == 0, "Old_pc(): iy_max == 0");
  FOR (iy, iy_max)
  FOR (ix, ix_max) {
    urect area {
      {ix * block_size.x, iy * block_size.y},
      {block_size.x, block_size.y}
    };
    process_block(dst, area);
  }
} // ()

void Old_pc::process_block(seze::Image &dst, CN(urect) area) {
  check();
  seze::RGB24 a, b;
  if (dither) {
    (*color_finder)(dst, area, a, b);
    a = (*palette_accepter)(a, pal);
    b = (*palette_accepter)(b, pal);
    Palette local_pal {a, b};
    (*dither)(dst, area, local_pal, palette_accepter, dither_power);
  } else {
    (*color_finder)(dst, area, a, b);
    for (uint y = area.pos.y; y < area.pos.y + area.size.y; ++y)
    for (uint x = area.pos.x; x < area.pos.x + area.size.x; ++x) {
      auto &col {dst.fast_get<seze::RGB24>(x, y)};
      col = (*color_selector)(col, a, b);
      col = (*palette_accepter)(col, pal);
    }
  }
} // process_block

void Old_pc::set_pal(CN(Palette) pal_)
  { pal = pal_; }

void Old_pc::set_block_size(CN(vec2u) block_size_)
  { block_size = block_size_; }

void Old_pc::set_color_finder(Color_finder* color_finder_)
  { color_finder = color_finder_; }

void Old_pc::set_color_selector(Color_selector* color_selector_)
  { color_selector = color_selector_; }

void Old_pc::set_palette_accepter(Palette_accepter* palette_accepter_)
  { palette_accepter = palette_accepter_; }

void Old_pc::set_dither(Dither* dither_, Real dither_power_) {
  dither = dither_;
  dither_power = dither_power_;
}

void Old_pc::check() const {
  iferror(pal.empty(), "Old_pc(): empty palette");
  iferror(block_size.x == 0 || block_size.y == 0,
    "Old_pc(): block_size is null len");
  iferror( !color_finder, "Old_pc(): color_finder is null");
  iferror( !color_selector, "Old_pc(): color_selector is null");
  iferror( !palette_accepter, "Old_pc(): palette_accepter is null");
}
