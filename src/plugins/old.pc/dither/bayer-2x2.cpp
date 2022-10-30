#include "bayer-2x2.hpp"
#include "image/image.hpp"
#include "old.pc/palette.accepter/palette-accepter.hpp"

void Dither_bayer_2x2::operator()(seze::Image &dst,
CN(urect) area, CN(Palette) pal,
Palette_accepter *palette_accepter, Real power) {
  constexpr static const Real matrix[2][2] {
    {-0.25, 0.25},
    { 0.5,  0},
  };
  const Real mul = (255.0f / pal.size()) * power;
  for (uint y {area.pos.y}; y < area.pos.y + area.size.y; ++y)
  for (uint x {area.pos.x}; x < area.pos.x + area.size.x; ++x) {
    auto &col = dst.fast_get<seze::RGB24>(x, y);
    col += mul * matrix[x % 2][y % 2];
    col = (*palette_accepter)(col, pal);
  }
}
