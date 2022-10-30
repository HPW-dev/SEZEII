#include "bayer-16x16.hpp"
#include "image/image.hpp"
#include "old.pc/palette.accepter/palette-accepter.hpp"

Dither_bayer_16x16::Dither_bayer_16x16() {
  Real tmp[16][16] {
    {255, 127, 223, 95 , 247, 119, 215, 87 , 253, 125, 221, 93 , 245, 117, 213, 85 },
    {63 , 191, 31 , 159, 55 , 183, 23 , 151, 61 , 189, 29 , 157, 53 , 181, 21 , 149},
    {207, 79 , 239, 111, 199, 71 , 231, 103, 205, 77 , 237, 109, 197, 69 , 229, 101},
    {15 , 143, 47 , 175, 7  , 135, 39 , 167, 13 , 141, 45 , 173, 5  , 133, 37 , 165},
    {243, 115, 211, 83 , 251, 123, 219, 91 , 241, 113, 209, 81 , 249, 121, 217, 89 },
    {51 , 179, 19 , 147, 59 , 187, 27 , 155, 49 , 177, 17 , 145, 57 , 185, 25 , 153},
    {195, 67 , 227, 99 , 203, 75 , 235, 107, 193, 65 , 225, 97 , 201, 73 , 233, 105},
    {3  , 131, 35 , 163, 11 , 139, 43 , 171, 1  , 129, 33 , 161, 9  , 137, 41 , 169},
    {252, 124, 220, 92 , 244, 116, 212, 84 , 254, 126, 222, 94 , 246, 118, 214, 86 },
    {60 , 188, 28 , 156, 52 , 180, 20 , 148, 62 , 190, 30 , 158, 54 , 182, 22 , 150},
    {204, 76 , 236, 108, 196, 68 , 228, 100, 206, 78 , 238, 110, 198, 70 , 230, 102},
    {12 , 140, 44 , 172, 4  , 132, 36 , 164, 14 , 142, 46 , 174, 6  , 134, 38 , 166},
    {240, 112, 208, 80 , 248, 120, 216, 88 , 242, 114, 210, 82 , 250, 122, 218, 90 },
    {48 , 176, 16 , 144, 56 , 184, 24 , 152, 50 , 178, 18 , 146, 58 , 186, 26 , 154},
    {192, 64 , 224, 96 , 200, 72 , 232, 104, 194, 66 , 226, 98 , 202, 74 , 234, 106},
    {0  , 128, 32 , 160, 8  , 136, 40 , 168, 2  , 130, 34 , 162, 10 , 138, 42 , 170},
  };
  // улучшение матрицы по формуле:
  // Mpre(i,j) = (Mint(i,j)+1) / n^2 - 0.5
  for (int y = 0; y < 16; ++y)
  for (int x = 0; x < 16; ++x) {
    matrix[x][y] = tmp[x][y];
    matrix[x][y] = (matrix[x][y] / float(16*16)) - 0.5f;
  }
}

void Dither_bayer_16x16::operator()(seze::Image &dst,
CN(urect) area, CN(Palette) pal,
Palette_accepter *palette_accepter, Real power) {
  const Real mul = (255.0f / pal.size()) * power;
  for (uint y {area.pos.y}; y < area.pos.y + area.size.y; ++y)
  for (uint x {area.pos.x}; x < area.pos.x + area.size.x; ++x) {
    auto &col = dst.fast_get<seze::RGB24>(x, y);
    col += mul * matrix[x % 16][y % 16];
    col = (*palette_accepter)(col, pal);
  }
}
