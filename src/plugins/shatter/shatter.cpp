extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/error.hpp"
#include "utils/random.hpp"
#include <map>
#include <mutex>

enum style_e { none, avg, mid, max, min, end, begin };

namespace {
  struct config_t {
    uint len = 10;
    bool random_len = false;
    style_e style = style_e::avg;
  } config = {};
  std::recursive_mutex rand_mu = {};
}

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Shaky motion blur";
  info.info = "Usage:\n"
    "-l, --len\t\tlength of motion [default: 10]\n"
    "-rl, --random-len\trandomize motion length in interval of -l value\n"
    "-s, --style\t\tstyle of blending: none, avg, mid, max,"
      "min, end, begin [default: avg]\n";
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
// parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_options({"-l", "--len"}); !str.empty())
    config.len = std::stoi(str);
  config.random_len = (parser.opt_exists("-rl")
    || parser.opt_exists("--random-len"));
  if (auto str = parser.get_options({"-s", "--style"}); !str.empty()) {
    std::map<std::string, style_e> table = {
      {"none", style_e::none},
      {"avg", style_e::avg},
      {"mid", style_e::mid},
      {"max", style_e::max},
      {"min", style_e::min},
      {"end", style_e::end},
      {"begin", style_e::begin},
    };
    try {
      config.style = table.at(str);
    } catch (...) {
      error("shatter init: bad name for shake style: " + str);
    }
  } // -s --style
  return info;
} // init

//! просто перемещает картинку без размытия
static void motion_simple(seze::Image& dst, CN(seze::Image) src,
float dir_x, float dir_y) {
  FOR (y, src.get_y())
  FOR (x, src.get_x()) {
    auto past_y = y + dir_y * config.len;
    auto past_x = x + dir_x * config.len;
    auto col = src.fast_get<seze::RGB24>(x, y);
    dst.set<seze::RGB24>(past_x, past_y, col);
  }
}

static float get_alpha_avg(float a, float b) { return 0.5f; }
static float get_alpha_mid(float a, float b) { return 0.5f; }
static float get_alpha_max(float a, float b) { return 0.5f; }
static float get_alpha_min(float a, float b) { return 0.5f; }
static float get_alpha_end(float a, float b) { return 0.5f; }
static float get_alpha_begin(float a, float b) { return 0.5f; }

//! перемещение картинки с размытием
template<auto get_alpha_f>
void motion_blur(seze::Image& dst, CN(seze::Image) src,
float dir_x, float dir_y) {
  FOR (y, src.get_y())
  FOR (x, src.get_x()) {
    auto past_y = y + dir_y * config.len;
    auto past_x = x + dir_x * config.len;
    auto col = src.fast_get<seze::RGB24>(x, y);
    dst.set<seze::RGB24>(past_x, past_y, col);
  }
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  seze::Image dst_pic(dst, mx, my, color_type);
  seze::Image buffer(dst_pic);
// единичный вектор направления:
  rand_mu.lock();
  auto rnd_f = seze::frand(-6, 6);
  rand_mu.unlock();
  auto dir_x = std::cos(rnd_f);
  auto dir_y = std::sin(rnd_f);
// движение картинки
  switch (config.style) {
    case style_e::avg: motion_blur<get_alpha_avg>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::mid: motion_blur<get_alpha_mid>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::max: motion_blur<get_alpha_max>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::min: motion_blur<get_alpha_min>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::end: motion_blur<get_alpha_end>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::begin: motion_blur<get_alpha_begin>(dst_pic, buffer, dir_x, dir_y); break;
    case style_e::none:
    default: motion_simple(dst_pic, buffer, dir_x, dir_y); break;
  }
} // core

void finalize() {}
