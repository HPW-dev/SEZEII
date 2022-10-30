#include "tvsim2bw.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

void Tvsim2bw::operator ()(CN(seze::Image) src, seze::Image &dst) {
  FOR (i, src.size) {
    auto col {src.fast_get<seze::RGB24>(i)};
    //col.R ^= 255;
    col.G ^= 255;
    col.B ^= 255;
    dst.fast_get<seze::RGB24>(i) = col;
  }
}
