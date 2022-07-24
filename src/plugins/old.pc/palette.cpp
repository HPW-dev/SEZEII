#include "palette.hpp"
#include "utils/macro.hpp"

void init_rgb1b(Palette &pal) {
  FOR (ir, 2)
    FOR (ig, 2)
      FOR (ib, 2)
        pal.push_back(seze::RGB24(ir*255, ig*255, ib*255));
}
