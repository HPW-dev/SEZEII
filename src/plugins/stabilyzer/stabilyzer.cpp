#include <omp.h>
#include <cstring>
#include <vector>
#include <cmath>
#include <numeric>
#include <numbers>
extern "C" {
#include "plugin-api.h"
}
//#include "utils/log.hpp"
#include "utils/macro.hpp"
#include "utils/aalloc.hpp"
#include "utils/error.hpp"
#include "Image/Image.hpp"
#include "Image/rgb24.hpp"
#include "Image/color.hpp"

struct Point { 
  int x=0, y=0;

  template <typename T>
  constexpr Point operator * (CN(T) val)
    { return {.x = x * val, .y = y * val}; }
};

using Real = float;
static bool first_init_flag = false;
static bool abcd_mode = false;
static seze::Image *big_buffer = {};
static seze::RGB24 color_a(0, 203, 255);
static seze::RGB24 color_b(208, 0, 255);
static seze::RGB24 color_c(255, 233, 0);
static seze::RGB24 color_d(0, 255, 0);
static byte threshold_r = 20;
static byte threshold_g = 20;
static byte threshold_b = 20;
static Point pos_a = {.x=0, .y=0};
static Point pos_b = {.x=20, .y=0};
static Point pos_c = {.x=0, .y=20};
static Point pos_d = {.x=20, .y=20};
static void (*rotate_f)(CN(seze::Image), seze::Image&, CN(Point), Real);
using v_pixel_t = std::vector<seze::RGB24>;
static void (*stretch_f)(CN(v_pixel_t), v_pixel_t&);
static void (*resize_f)(CN(seze::Image), seze::Image&, CN(Point));

static void first_init(int mx, int my, color_t color_type) {
  if (first_init_flag)
    return;
  auto size = std::max(mx, my) * 1.35;
  big_buffer = new seze::Image(size, size, color_type);
  first_init_flag = true;
}

static bool check_color(CN(seze::RGB24) a, CN(seze::RGB24) b) {
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

static void find_point(CN(seze::Image) src, Point& dst,
CN(seze::RGB24) color) {
  FOR (y, src.Y)
  FOR (x, src.X)
    if (check_color(src.fast_get<seze::RGB24>(x, y), color)) {
      dst.x = x;
      dst.y = y;
      return;
    }
} // find_point

static void find_points(CN(seze::Image) src) {
  find_point(src, pos_a, color_a);
  find_point(src, pos_b, color_b);
  find_point(src, pos_c, color_c);
  find_point(src, pos_d, color_d);
  /*LOG("a.x:" << pos_a.x << " a.y:" << pos_a.y <<
    " b.x:" << pos_b.x << " b.y:" << pos_b.y <<
    " c.x:" << pos_c.x << " c.y:" << pos_c.y <<
    " d.x:" << pos_d.x << " d.y:" << pos_d.y << "\n");*/
}

//! узнаеёт угол между точкой a и b
static Real get_angle(CN(Point) a, CN(Point) b) {
  Point diff{.x=b.x - a.x, .y=b.y - a.y};
  Real len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
  if (len <= 0)
    return 0;
  len = 1.0 / len; // оптимизация деления
  return std::atan2(diff.x * len, diff.y * len);
}

static Point calc_center(CN(seze::Image) dst, CN(seze::Image) src)
  { return {.x = dst.X / 2 - src.X / 2, .y = dst.Y / 2 - src.Y / 2}; }

constexpr Real to_radian(Real x) { 
  constexpr Real mul = std::numbers::pi / 180.0;
  return x * mul;
}

static seze::RGB24 lerp(CN(seze::RGB24) a, CN(seze::RGB24) b, Real t) {
  return seze::RGB24 {
    byte(std::lerp(a.R, b.R, t)),
    byte(std::lerp(a.G, b.G, t)),
    byte(std::lerp(a.B, b.B, t)),
  };
}

static seze::RGB24 blerp(CN(seze::RGB24) c00, CN(seze::RGB24) c10,
CN(seze::RGB24) c01, CN(seze::RGB24) c11, Real tx, Real ty)
  { return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty); }

inline seze::RGB24 bilinear_core(CN(seze::Image) src, Real gx, Real gy) {
  int ix = gx;
	int iy = gy;
	Real fx = gx - ix;
	Real fy = gy - iy;
  auto c00 = src.get<seze::RGB24>(ix+0, iy+0);
  auto c01 = src.get<seze::RGB24>(ix+0, iy+1);
  auto c10 = src.get<seze::RGB24>(ix+1, iy+0);
  auto c11 = src.get<seze::RGB24>(ix+1, iy+1);
  return blerp(c00, c10, c01, c11, fx, fy);
}

inline seze::RGB24 neighbor_core(CN(seze::Image) src, Real gx, Real gy)
  { return src.get<seze::RGB24>(int(gx), int(gx)); }

inline seze::RGB24 neighbor_1d_core(CN(seze::Image) src, Real gx, Real gy)
  { return {}; /* TODO */ }

inline seze::RGB24 linear_core(CN(seze::Image) src, Real gx, Real gy)
  { return {}; /* TODO */ }

template <auto core>
void rotate_templ(CN(seze::Image) src, seze::Image& dst,
CN(Point) pos, Real angle) {
  Real mul_x = std::cos(angle);
  Real mul_y = std::sin(angle);
  FOR (y, dst.Y)
  FOR (x, dst.X) {
    auto rot_x = mul_x * (x - pos.x) - mul_y * (y - pos.y) + pos.x;
    auto rot_y = mul_y * (x - pos.x) + mul_x * (y - pos.y) + pos.y;
    auto pix = core(src, rot_x, rot_y);
  }
} // rotate_templ

template <auto core>
void resize_templ(CN(seze::Image) src, seze::Image& dst, 
CN(Point) pos) {
  auto scale_x = Real(src.X) / dst.X;
	auto scale_y = Real(src.Y) / dst.Y;
  FOR (y, dst.Y)
  FOR (x, dst.X) {
    auto src_x = x * scale_x - pos.x;
    auto src_y = y * scale_y - pos.y;
    dst.fast_set<seze::RGB24>(x, y, core(src, src_x, src_y));
  }
} // resize_templ

//! растягивает один 1D массив цветов в другой
template <auto core>
void stretch_templ(CN(v_pixel_t) src, v_pixel_t& dst) {
  // TODO
}

#define rotate_fast rotate_templ<neighbor_core>
#define rotate_bilinear rotate_templ<bilinear_core>
#define resize_fast resize_templ<neighbor_core>
#define resize_bilinear resize_templ<bilinear_core>
#define stretch_fast stretch_templ<neighbor_1d_core>
#define stretch_linear stretch_templ<linear_core>

static void rotate_point(CN(Point) center, Point& dst, Real angle) {
  Real mul_x = std::cos(angle);
  Real mul_y = std::sin(angle);
  dst.x = mul_x * (dst.x - center.x) - mul_y * (dst.x - center.y) + center.x;
  dst.y = mul_y * (dst.y - center.x) + mul_x * (dst.y - center.y) + center.y;
}

static void abcd_stabilyze(seze::Image& dst) {

} // abcd_stabilyze

static void abc_stabilyze(seze::Image& dst) {
  /* abc mode:
  * 1. определение точек:
      A ------- B
      |
      |
      |
      C
  * 2. Поворот картинки, чтобы B был справа от A
  * 3. B и C тоже поворачиваются
  * 4. Сдвиг строк, чтобы C был снизу A
  * 5. Ресайз до размеров вывода */
  /*FOR (y, dst.Y)
  FOR (x, dst.X)
    big_buffer->fast_set<seze::RGB24>(x, y, dst.fast_get<seze::RGB24>(x, y));
  find_points(dst);
  auto angle = to_radian(90) - calc_ab_angle();
  auto center = calc_center(*big_buffer, dst);
  rotate_f(dst, *big_buffer, center, angle);
  rotate_point(pos_a, pos_b, angle);
  rotate_point(pos_a, pos_c, angle);
  auto shift_val = get_ac_shift_value(dst.Y);
  //pos_c.x += shift_val * my;
  shift_f(*big_buffer, shift_val);
  resize_f(*big_buffer, dst, pos_a * -1,
    big_buffer->X - pos_a.x + (big_buffer->X - pos_b.x),
    big_buffer->Y - pos_a.y + (big_buffer->Y - pos_c.y));*/
} // abc_stabilyze

struct PluginInfo init(CP(char) options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Video stabilyzer";
  info.version = 3;
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  rotate_f = &rotate_fast;
  stretch_f = &stretch_fast;
  resize_f = &resize_fast;
  return info;
} // init

void core(byte* dst, int mx, int my, int /*stride*/,
color_t color_type) {
  first_init(mx, my, color_type);
  auto buffer = seze::Image(dst, mx, my, color_type);
  if (abcd_mode)
    abcd_stabilyze(buffer);
  else
    abc_stabilyze(buffer);
} // core

void finalize() {
  delete big_buffer;
}
