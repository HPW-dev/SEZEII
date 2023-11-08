#include <omp.h>
#include <cassert>
#include <array>
#include <memory>
#include <mutex>
#include "plugin-api.h"
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/log.hpp"
#include "utils/random.hpp"

namespace config {
  inline float fade_speed {0.2}; /// скорость затухания предыдущего кадра
  inline float jitter_percent {0}; /// степень съёживания по высоте
  inline uint blur_x {6}; /// блюр по ширине
  inline uint blur_y {2}; /// блюр по высоте
  inline double sharpen_power {5}; /// сила фильтра резкости
}

static std::unique_ptr<seze::Image> dst_frame {};
static std::unique_ptr<seze::Image> prev_frame {};
static std::unique_ptr<seze::Image> sharpen_frame {};
static std::once_flag once_init_frames {};

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Simple VHS";
  info.info = "Usage:\n"
    /*"-c, --contrast\t\timage contrast\n"
    "-b, --brightness\timage brightness\n"
    "-p, --palette\t\tpalette file (.png)\n"*/;

// parse opts:
  CmdParser parser(options);
  /*if (auto str = parser.get_options({"-c", "--contrast"}); !str.empty())
    config::contrast = std::clamp<float>(std::stof(str), 0, 20);
  if (auto str = parser.get_options({"-b", "--brightness"}); !str.empty())
    config::brightness = std::clamp<float>(std::stof(str), -1, 1);
  if (auto str = parser.get_options({"-p", "--palette"}); !str.empty()) {
    init_palette(str);
  } else {
    error("need -p parameter (see usage)\n");
  }*/

  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

inline void accept_fade(seze::Image& dst) {
  cfor (i, dst.bytes)
    dst.get_data()[i] = std::max(dst.get_cdata()[i], prev_frame->get_cdata()[i]);
  // предыдущий кадр становится текущим с затуханием
  cfor (i, dst.bytes)
    prev_frame->get_data()[i] = std::max<int>(0, int(dst.get_cdata()[i]) - (255 * config::fade_speed));
}

inline void accept_jitter(seze::Image& dst) {
  // TODO почини
  return_if(config::jitter_percent == 0);
  uint y_idx = 0;
  cfor (y, dst.Y) {
    if (config::jitter_percent < seze::frand()) {
      ++y_idx;
      cfor (x, dst.X) {
        auto src = dst.fast_get<seze::RGB24>(x, y);
        dst.fast_set<seze::RGB24>(x, y_idx, src);
      }
    }
  }
  for (auto y = y_idx; y < dst.Y; ++y)
    cfor (x, dst.X)
      dst.fast_set<seze::RGB24>(x, y, {});
} // accept_jitter

inline void accept_blur(seze::Image& dst) {
  // блюр по ширине
  if (config::blur_x != 0) {
    #pragma omp parallel for simd collapse(2)
    cfor (y, dst.Y)
    cfor (x, dst.X) {
      int total_r = 0;
      int total_g = 0;
      int total_b = 0;
      cfor (wnd_x, config::blur_x) {
        auto sample = dst.get<seze::RGB24>(x + wnd_x, y, seze::boundig_e::mirror);
        total_r += sample.R;
        total_g += sample.G;
        total_b += sample.B;
      }
      total_r /= config::blur_x;
      total_g /= config::blur_x;
      total_b /= config::blur_x;
      dst.fast_set<seze::RGB24>(x, y, seze::RGB24(total_r, total_g, total_b));
    } // for x y
  }

  // блюр по высоте
  if (config::blur_y != 0) {
    #pragma omp parallel for simd collapse(2)
    cfor (y, dst.Y)
    cfor (x, dst.X) {
      int total_r = 0;
      int total_g = 0;
      int total_b = 0;
      cfor (wnd_y, config::blur_y) {
        auto sample = dst.get<seze::RGB24>(x, y + wnd_y, seze::boundig_e::mirror);
        total_r += sample.R;
        total_g += sample.G;
        total_b += sample.B;
      }
      total_r /= config::blur_y;
      total_g /= config::blur_y;
      total_b /= config::blur_y;
      dst.fast_set<seze::RGB24>(x, y, seze::RGB24(total_r, total_g, total_b));
    } // for x y
  }
} // accept_blur

seze::RGBf convert(CN(seze::RGB24) src) {
  return seze::RGBf(
    src.R / 255.0,
    src.G / 255.0,
    src.B / 255.0);
}

inline void accept_sharpen(seze::Image& dst) {
  double p = config::sharpen_power;
  std::array<double, 9> filter {
    -p/8, -p/8, -p/8,
    -p/8,  p+1, -p/8,
    -p/8, -p/8, -p/8
  };

  cfor (i, dst.bytes)
    sharpen_frame->get_data()[i] = dst.get_cdata()[i];
  auto get_r = [](int x, int y)->int { return sharpen_frame->get<seze::RGB24>(x, y).R; };
  auto get_g = [](int x, int y)->int { return sharpen_frame->get<seze::RGB24>(x, y).G; };
  auto get_b = [](int x, int y)->int { return sharpen_frame->get<seze::RGB24>(x, y).B; };

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    auto pix_r =
      get_r(x-1, y+0) * filter[0] + get_r(x+0, y+0) * filter[1] + get_r(x+1, y+0) * filter[2] +
      get_r(x-1, y+1) * filter[3] + get_r(x+0, y+1) * filter[4] + get_r(x+1, y+1) * filter[5] +
      get_r(x-1, y+2) * filter[6] + get_r(x+0, y+2) * filter[7] + get_r(x+1, y+2) * filter[8];
    auto pix_g =
      get_g(x-1, y+0) * filter[0] + get_g(x+0, y+0) * filter[1] + get_g(x+1, y+0) * filter[2] +
      get_g(x-1, y+1) * filter[3] + get_g(x+0, y+1) * filter[4] + get_g(x+1, y+1) * filter[5] +
      get_g(x-1, y+2) * filter[6] + get_g(x+0, y+2) * filter[7] + get_g(x+1, y+2) * filter[8];
    auto pix_b =
      get_b(x-1, y+0) * filter[0] + get_b(x+0, y+0) * filter[1] + get_b(x+1, y+0) * filter[2] +
      get_b(x-1, y+1) * filter[3] + get_b(x+0, y+1) * filter[4] + get_b(x+1, y+1) * filter[5] +
      get_b(x-1, y+2) * filter[6] + get_b(x+0, y+2) * filter[7] + get_b(x+1, y+2) * filter[8];
    dst.fast_set<seze::RGB24>(x, y, seze::RGB24(
      std::clamp<float>(pix_r, 0, 255),
      std::clamp<float>(pix_g, 0, 255),
      std::clamp<float>(pix_b, 0, 255)
    ));
  }
} // accept_sharpen

void process(seze::Image& dst) {
  accept_jitter(dst);
  accept_blur(dst);
  accept_sharpen(dst);
  accept_fade(dst);
} // process

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  assert(dst);
  assert(color_type == color_t::seze_RGB24);

  auto init_frames = [dst, mx, my, color_type]{
    dst_frame = std::make_unique<seze::Image>(dst, mx, my, color_type);
    prev_frame = std::make_unique<seze::Image>(mx, my, color_type);
    sharpen_frame = std::make_unique<seze::Image>(mx, my, color_type);
  };
  std::call_once(once_init_frames, init_frames);

  process(*dst_frame);
}

void finalize() {}
