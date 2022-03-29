#include <omp.h>
extern "C" {
#include "plugin-api.h"
}
#include "global.hpp"
#include "image/image.hpp"
#include "math.hpp"
#include "util.hpp"

static void first_init(int mx, int my, color_t color_type) {
  if (first_init_flag)
    return;
  auto size = std::max(mx, my) * 1.35;
  big_buffer = new seze::Image(size, size, color_type);
  first_init_flag = true;
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
  * 2. Поворот картинки, чтобы B был справа от A. Точки тоже повернуть
  * 3. вставить картинку влево вверх, чтоы A стал нулём
  * 4. растянуть C и B до dst.Y и dst.X */
  find_points(dst);
  auto ab_rotation = to_radian(90) - radian_btw_point(pos_a, pos_b);
  auto center = calc_center(*big_buffer, dst);
  rotate_f(dst, *big_buffer, center, ab_rotation);
  rotate_point(pos_a, pos_b, ab_rotation);
  rotate_point(pos_a, pos_c, ab_rotation);
  resize_f(*big_buffer, dst, pos_a * -1,
    big_buffer->X + (big_buffer->X - pos_b.x) - pos_a.x,
    big_buffer->Y + (big_buffer->Y - pos_c.y) - pos_a.y);
} // abc_stabilyze

struct PluginInfo init(CP(char) options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Video stabilyzer";
  info.version = 3;
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  rotate_f = &rotate_bicubic;
  stretch_f = &stretch_fast;
  resize_f = &resize_bicubic;
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
