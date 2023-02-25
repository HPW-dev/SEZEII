extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/error.hpp"
//#include "utils/log.hpp"
#include "utils/random.hpp"
#include <map>
#include <mutex>

enum style_e { none, avr, mid, max, min, end, begin, light };

namespace {
  struct config_t {
    int len = 16;
    bool random_len = false;
    style_e style = style_e::avr;
    float angle = 0;
    bool use_angle = false;
    float light_ratio = 0.4f;
    seze::boundig_e bounding_mode = seze::boundig_e::clamp;
  } config = {};
  std::recursive_mutex rand_mu = {};
}

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Shaky motion blur";
  info.info = "Usage:\n"
    "-l, --len\t\tlength of motion [default: 16]\n"
    "-rl, --random-len\trandomize motion length in interval of -l value\n"
    "-s, --style\t\tstyle of blending: none, avr, mid, max,"
      "min, end, begin [default: avr]\n"
    "-a, --angle\t\tcustom motion angle\n"
    "-lr, --light-ratio\tmultipler for light style [default: 0.4]\n"
    "-m, --mirror\t\tenable mirror image bounding\n"
  ;
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
// parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_options({"-l", "--len"}); !str.empty())
    config.len = std::stoi(str);
  iferror(config.len < 2, "shatter init: config.len < 2");
  config.random_len = (parser.opt_exists("-rl")
    || parser.opt_exists("--random-len"));
  if (auto str = parser.get_options({"-s", "--style"}); !str.empty()) {
    std::map<std::string, style_e> table = {
      {"none", style_e::none},
      {"avr", style_e::avr},
      {"mid", style_e::mid},
      {"max", style_e::max},
      {"min", style_e::min},
      {"end", style_e::end},
      {"begin", style_e::begin},
      {"light", style_e::light},
    };
    try {
      config.style = table.at(str);
    } catch (...) {
      error("shatter init: bad name for shake style: " + str);
    }
    if (auto str = parser.get_options({"-a", "--angle"}); !str.empty()) {
      config.angle = std::stoi(str);
      config.use_angle = true;
    }
    if (auto str = parser.get_options({"-lr", "--light-ratio"}); !str.empty())
      config.light_ratio = std::stof(str);
    if (parser.opt_exists("-m") || parser.opt_exists("--mirror"))
      config.bounding_mode = seze::boundig_e::mirror;
  } // -s --style
  return info;
} // init

void set_dir_by_angle(float& dir_x, float& dir_y, float angle) {
  constexpr auto pi = 3.14159265359f;
  auto radian = angle * (pi / 180.0f);
  dir_x = std::cos(radian);
  dir_y = std::sin(radian);
}

//! просто перемещает картинку без размытия
static void motion_simple(seze::Image& dst, CN(seze::Image) src,
float dir_x, float dir_y) {
  auto len = config.len;
  if (config.random_len)
    len = seze::irand(0, config.len);
  if (len < 1)
    return;
  cfor (y, src.Y)
  cfor (x, src.X) {
    auto past_y = y + dir_y * len;
    auto past_x = x + dir_x * len;
    auto col = src.fast_get<seze::RGB24>(x, y);
    dst.set<seze::RGB24>(past_x, past_y, col);
  }
} // motion_simple

constexpr float blend(float dst, float src, float alpha) { return dst + (src - dst) * alpha; }
constexpr void   blend_avr(float& dst, float src, float    , float     ) { dst = blend(dst, src, 0.5f); }
constexpr void   blend_max(float& dst, float src, float    , float     ) { dst = std::max(dst, src); }
constexpr void   blend_min(float& dst, float src, float    , float     ) { dst = std::min(dst, src); }
constexpr void   blend_end(float& dst, float src, float len, float path) { dst = blend(dst, src, path/len); }
constexpr void blend_begin(float& dst, float src, float len, float path) { dst = blend(dst, src, 1.0f - path/len); }
void           blend_light(float& dst, float src, float len, float path) { dst = dst + src * config.light_ratio; }
constexpr void   blend_mid(float& dst, float src, float len, float path) { 
  auto alpha = path/len * 2.0f;
  if (alpha > 1.0f)
    alpha = 1.0f - (alpha - 1.0f);
  dst = blend(dst, src, alpha); 
}

// RGB24 byte to float [0..1]
static seze::RGBf convert(CN(seze::RGB24) in) {
  constexpr float fmul = 1.f / 255.f;
  return seze::RGBf(in.R * fmul, in.G * fmul, in.B * fmul);
}

// float [0..1] to RGB24 byte
static seze::RGB24 convert(CN(seze::RGBf) in) {
  auto R = std::clamp(in.R * 255.f, 0.f, 255.f);
  auto G = std::clamp(in.G * 255.f, 0.f, 255.f);
  auto B = std::clamp(in.B * 255.f, 0.f, 255.f);
  return seze::RGB24(R, G, B);
}

//! перемещение картинки с размытием
template<auto get_alpha_f>
void motion_blur(seze::Image& dst, CN(seze::Image) src,
float dir_x, float dir_y) {
  auto len = config.len;
  if (config.random_len)
    len = seze::irand(0, config.len);
  if (len < 1)
    return;
  cfor (y, src.Y)
  cfor (x, src.X) {
    auto c1 = src.fast_get<seze::RGB24>(x, y);
    seze::RGBf fc1 = convert(c1);
    seze::RGBf accum;
    int path = 0;
    while (path < len) {
      ++path;
      auto get_x = x + path * dir_x;
      auto get_y = y + path * dir_y;
      auto c2 = src.get<seze::RGB24>(get_x, get_y, config.bounding_mode);
      seze::RGBf fc2 = convert(c2);
      get_alpha_f(fc1.R, fc2.R, len, path);
      get_alpha_f(fc1.G, fc2.G, len, path);
      get_alpha_f(fc1.B, fc2.B, len, path);
      accum.R += fc1.R;
      accum.G += fc1.G;
      accum.B += fc1.B;
    }
    accum.R /= float(len);
    accum.G /= float(len);
    accum.B /= float(len);
    c1 = convert(accum);
    dst.fast_set<seze::RGB24>(x, y, c1);
  }
} // motion_blur

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  seze::Image dst_pic(dst, mx, my, color_type);
  seze::Image buffer(dst_pic);
// единичный вектор направления:
  rand_mu.lock();
  auto rnd_i = seze::irand();
  rand_mu.unlock();
  float dir_x, dir_y;
  if (config.use_angle) {
    set_dir_by_angle(dir_x, dir_y, config.angle);
  } else {
    dir_x = std::cos(float(rnd_i));
    dir_y = std::sin(float(rnd_i));
  }
// движение картинки
  switch (config.style) {
    case style_e::avr: motion_blur<blend_avr>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::mid: motion_blur<blend_mid>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::max: motion_blur<blend_max>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::min: motion_blur<blend_min>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::end: motion_blur<blend_end>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::begin: motion_blur<blend_begin>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::light: motion_blur<blend_light>(dst_pic, buffer, dir_x, dir_y); break;
    default:
    case style_e::none: motion_simple(dst_pic, buffer, dir_x, dir_y); break;
  }
} // core

void finalize() {}
