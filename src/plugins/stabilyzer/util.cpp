#include <vector>
#include <cstring>
#include "util.hpp"
#include "image/color.hpp"
#include "utils/log.hpp"

bool check_color(CN(seze::RGB24) a, CN(seze::RGB24) b) {
  auto diff = std::abs(int(a.R) - b.R);
  if (diff > threshold_r)
    return false;
  diff = std::abs(int(a.G) - b.G);
  if (diff > threshold_g)
    return false;
  diff = std::abs(int(a.B) - b.B);
  if (diff > threshold_b)
    return false;
  return true;
}

void find_point(CN(seze::Image) src, Point& dst,
CN(seze::RGB24) color) {
  FOR (y, src.Y)
  FOR (x, src.X)
    if (check_color(src.fast_get<seze::RGB24>(x, y), color)) {
      dst.x = x;
      dst.y = y;
      return;
    }
} // find_point

void find_points(CN(seze::Image) src) {
  find_point(src, pos_a, color_a);
  find_point(src, pos_b, color_b);
  find_point(src, pos_c, color_c);
  find_point(src, pos_d, color_d);
  /*LOG("a.x:" << pos_a.x << " a.y:" << pos_a.y <<
    " b.x:" << pos_b.x << " b.y:" << pos_b.y <<
    " c.x:" << pos_c.x << " c.y:" << pos_c.y <<
    " d.x:" << pos_d.x << " d.y:" << pos_d.y << "\n");*/
}

Point calc_center(CN(seze::Image) dst, CN(seze::Image) src)
  { return {.x = dst.X / 2 - src.X / 2, .y = dst.Y / 2 - src.Y / 2}; }


void stretch(seze::Image& dst) {
  // буфферы строк и столбцов для оптимизации
  v_pixel_t arr_x_src(dst.X);
  v_pixel_t arr_x_dst(dst.X);
  v_pixel_t arr_y_src(dst.Y);
  v_pixel_t arr_y_dst(dst.Y);
  auto arr_x_bytes = seze::get_size(color_t::seze_BGR24) * dst.X;
  auto arr_y_bytes = seze::get_size(color_t::seze_BGR24) * dst.Y;
  // горизонтальное растягивание
  FOR (y, dst.Y) {
    memcpy(arr_x_src.data(), &dst.fast_get<seze::RGB24>(0, y), arr_x_bytes);
    memcpy(arr_x_dst.data(), arr_x_src.data(), arr_x_bytes);
    auto shift_x = (Real(-pos_c.x) / dst.Y) * y;
    auto shift_x_ed = (Real(dst.X - pos_d.x) / dst.Y) * y;
    auto size_x = dst.X + shift_x_ed - shift_x;
    stretch_f(arr_x_src, arr_x_dst, shift_x, size_x);
    memcpy(&dst.fast_get<seze::RGB24>(0, y), arr_x_dst.data(), arr_x_bytes);
  }
  // вертикальное растягивание
  FOR (x, dst.X) {
    FOR (y, dst.Y)
      arr_y_src[y] = dst.fast_get<seze::RGB24>(x, y);
    memcpy(arr_y_dst.data(), arr_y_src.data(), arr_y_bytes);
    auto shift_y_ed = (Real(dst.Y - pos_d.y) / dst.X) * x;
    auto size_y = dst.Y + shift_y_ed;
    stretch_f(arr_y_src, arr_y_dst, 0, size_y);
    FOR (y, dst.Y)
      dst.fast_set<seze::RGB24>(x, y, arr_y_dst[y]);
  }
} // stretch
