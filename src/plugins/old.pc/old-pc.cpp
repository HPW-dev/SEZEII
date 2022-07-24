#include "old-pc.hpp"
#include "image/image.hpp"
#include "utils/error.hpp"
#include "color-finder.hpp"
#include "color-selector.hpp"
#include "palette-accepter.hpp"

Old_pc::Old_pc(CN(Palette) pal_, CN(vec2u) block_size_,
Color_finder *color_finder_, Color_selector *color_selector_,
Palette_accepter *palette_accepter_)
: pal{pal_}
, block_size{block_size_}
, color_finder{color_finder_}
, color_selector{color_selector_}
, palette_accepter{palette_accepter_} {
  iferror(pal.empty(), "Old_pc(): empty palette");
  iferror(block_size.x == 0 || block_size.y == 0,
    "Old_pc(): block_size is null len");
}

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
  seze::RGB24 a, b;
  (*color_finder)(dst, area, a, b);
  //a = (*palette_accepter)(a, pal);
  //b = (*palette_accepter)(b, pal);
  for (uint y = area.pos.y; y < area.pos.y + area.size.y; ++y)
  for (uint x = area.pos.x; x < area.pos.x + area.size.x; ++x) {
    auto &col {dst.fast_get<seze::RGB24>(x, y)};
    //col = (*palette_accepter)(col, pal);
    col = (*color_selector)(col, a, b);
    col = (*palette_accepter)(col, pal);
  }
} // process_block
