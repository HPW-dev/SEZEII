#include <algorithm>
#include "tvsim2bw.hpp"
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

Tvsim2bw::Tvsim2bw()
: bw_img {make_shared_p<seze::Image>()}
, bw_img_scaled {make_shared_p<seze::Image>()}
{}

void Tvsim2bw::operator ()(CN(seze::Image) src, seze::Image &dst) {
  bw_img->init(src.X, src.Y, seze_f_gray);
  desaturate(src, *bw_img, desat_type);
  bw_img_scaled->init(scale_wh.x, scale_wh.y, seze_f_gray);
  scale_gray(*bw_img, *bw_img_scaled, scale_type_in);
  // TODO
  scale_gray(*bw_img_scaled, *bw_img, scale_type_out);
  gray_to_rgb24(*bw_img, dst);
}
