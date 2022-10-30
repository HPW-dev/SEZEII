#include <algorithm>
#include "tvsim2bw.hpp"
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

void Tvsim2bw::operator ()(CN(seze::Image) src, seze::Image &dst) {
  std::call_once(once_flag, [this, &src]() {
    bw_img = make_shared_p<seze::Image>(src.X, src.Y, seze_f_gray);
  });
  desaturate(src, *bw_img, desat_type);
  gray_to_rgb24(*bw_img, dst);
}
