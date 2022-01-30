extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
//#include "utils/log.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include <map>

using namespace seze;

enum class star_mode_t { average=0, max3, rgb, red, green, blue };

namespace config {
  float len = 32.0f;
  float power = 1.5f;
  int threshold = 196;
  float swapn_percent = 1.0f;
  bool random_spawn = true;
  bool random_size = true;
  star_mode_t mode = star_mode_t::rgb;
  bool no_v = false;
  bool no_h = false;
  bool no_d = false;
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Lighten stars";
  info.info = "Usage:\n"
    "-l, --len\tlength of star beams\n"
    "-p, --power\tstar lightnes power\n"
    "-t, --threshold\tthreshold value of color [0..254]\n"
    "-r, --rspawn\tenable random star swapning\n"
    "-s, --spawn\tpercent of spawn star (use with -r)\n"
    "--rsize\t\tenable random star size\n"
    "-m, --mode\tdrawing mode. Avaliable: avr, max3, rgb, red, green, blue\n"
    "--nv\tdisable vertical beams\n"
    "--nh\tdisable horizontal beams\n"
    "--nd\tdisable diagonal beams\n";
// parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_options({"-l", "--len"}); !str.empty())
    config::len = std::stof(str);
  if (auto str = parser.get_options({"-p", "--power"}); !str.empty())
    config::power = std::stof(str);
  if (auto str = parser.get_options({"-t", "--threshold"}); !str.empty())
    config::threshold = std::clamp(std::stoi(str), 0, 255);
  if (auto str = parser.get_options({"-s", "--spawn"}); !str.empty())
    config::swapn_percent = std::clamp(std::stof(str), 0.f, 100.f);
  config::random_spawn = (parser.opt_exists("-r") or parser.opt_exists("--rspawn"));
  config::no_v = parser.opt_exists("--nv");
  config::no_h = parser.opt_exists("--nh");
  config::no_d = parser.opt_exists("--nd");
  config::random_size = parser.opt_exists("--rsize");
  if (auto str = parser.get_options({"-m", "--mode"}); !str.empty()) {
    std::map<std::string, star_mode_t> table = {
      {"avr", star_mode_t::average},
      {"max3", star_mode_t::max3},
      {"rgb", star_mode_t::rgb},
      {"red", star_mode_t::red},
      {"green", star_mode_t::green},
      {"blue", star_mode_t::blue},
    };
    try {
      config::mode = table.at(str);
    } catch (...) {
      error("init: star draw mode mapping error");
    }
  }
  if (config::random_spawn or config::random_size) {
    seze::randomize_seed();
    bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  }
    bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

static int get_average(CN(seze::RGB24) in)
{ return (in.R + in.G + in.B) / 3; }

static int get_max3(CN(seze::RGB24) in)
{ return std::max(in.B, std::max(in.R, in.G)); }

void blend(seze::RGB24& dst, CN(seze::RGB24) src, float alpha) {
  dst.R = dst.R + (src.R - dst.R) * alpha;
  dst.G = dst.G + (src.G - dst.G) * alpha;
  dst.B = dst.B + (src.B - dst.B) * alpha;
}

void blend_r(seze::RGB24& dst, byte component, float alpha)
{ dst.R = dst.R + (component - dst.R) * alpha; }

void blend_g(seze::RGB24& dst, byte component, float alpha)
{ dst.G = dst.G + (component - dst.G) * alpha; }

void blend_b(seze::RGB24& dst, byte component, float alpha)
{ dst.B = dst.B + (component - dst.B) * alpha; }


template <auto func> static void stars_full(seze::Image& dst) {
  auto buffer(dst);
  auto local_threshold = 255.0f - config::threshold;
  iferror(local_threshold <= 0, "stars_full: 255.0f - threshold <= 0");
  int ilen_bnd = std::ceil(config::len);
  auto threshold = config::threshold;
  auto my = dst.get_y();
  auto mx = dst.get_x();
  for (int y = ilen_bnd; y < my - ilen_bnd; ++y)
  for (int x = ilen_bnd; x < mx - ilen_bnd; ++x) {
    auto col = buffer.fast_get<seze::RGB24>(x, y);
    auto value = func(col);
    if (value > threshold) {
      float flen2 = config::len;
      if (config::random_spawn
      and seze::frand(0.0f, 100.0f) > config::swapn_percent)
        continue;
      if (config::random_size)
        flen2 = seze::frand(0, flen2);
      int ilen = std::ceil(flen2);
      // draw star
      value -= threshold;
      auto power = value / local_threshold;
      for (int i = 1; i < ilen; ++i) {
        float fi = i / flen2;
        auto dist_ratio = std::lerp(1.0f, 0.0f, fi);
        auto alpha = dist_ratio * power * config::power;
        alpha = std::min(1.0f, alpha);
        if ( !config::no_v) {
          blend(dst.fast_get<seze::RGB24>(x, y-i), col, alpha);
          blend(dst.fast_get<seze::RGB24>(x, y+i), col, alpha);
        }
        if ( !config::no_h) {
          blend(dst.fast_get<seze::RGB24>(x-i, y), col, alpha);
          blend(dst.fast_get<seze::RGB24>(x+i, y), col, alpha);
        }
      } // for ilen
      // diagonal beams
      if (config::no_d)
        continue;
      auto ilen_half = ilen / 2;
      auto flen2_half = flen2 / 2.0f;
      if (ilen_half < 1 or flen2_half <= 0.0f)
        continue;
      for (int i = 1; i < ilen_half; ++i) {
        float fi = i / flen2_half;
        auto dist_ratio = std::lerp(1.0f, 0.0f, fi);
        auto alpha = dist_ratio * power * config::power;
        alpha = std::min(1.0f, alpha);
        blend(dst.fast_get<seze::RGB24>(x-i, y-i), col, alpha);
        blend(dst.fast_get<seze::RGB24>(x+i, y+i), col, alpha);
        blend(dst.fast_get<seze::RGB24>(x+i, y-i), col, alpha);
        blend(dst.fast_get<seze::RGB24>(x-i, y+i), col, alpha);
      } // for ilen
    } // if val > thrsld
  } // for y/x
} // stars_full

static void stars_rgb(seze::Image& dst, bool use_red, bool use_green, bool use_blue) {
  auto buffer(dst);
  auto local_threshold = 255.0f - config::threshold;
  iferror(local_threshold <= 0, "stars_full: 255.0f - threshold <= 0");
  int ilen_bnd = std::ceil(config::len);
  auto threshold = config::threshold;
  auto my = dst.get_y();
  auto mx = dst.get_x();
  for (int y = ilen_bnd; y < my - ilen_bnd; ++y)
  for (int x = ilen_bnd; x < mx - ilen_bnd; ++x) {
    auto col = buffer.fast_get<seze::RGB24>(x, y);
    int value_r = col.R;
    int value_g = col.G;
    int value_b = col.B;
    auto func = [&](int value, auto blend_f, byte COL)->bool {
      if (value > threshold) {
        float flen2 = config::len;
        if (config::random_spawn
        and seze::frand(0.0f, 100.0f) > config::swapn_percent)
          return false;
        if (config::random_size)
          flen2 = seze::frand(0, flen2);
        int ilen = std::ceil(flen2);
        // draw star
        value -= threshold;
        auto power = value / local_threshold;
        for (int i = 1; i < ilen; ++i) {
          float fi = i / flen2;
          auto dist_ratio = std::lerp(1.0f, 0.0f, fi);
          auto alpha = dist_ratio * power * config::power;
          alpha = std::min(1.0f, alpha);
          if ( !config::no_v) {
            blend_f(dst.fast_get<seze::RGB24>(x, y-i), COL, alpha);
            blend_f(dst.fast_get<seze::RGB24>(x, y+i), COL, alpha);
          }
          if ( !config::no_h) {
            blend_f(dst.fast_get<seze::RGB24>(x-i, y), COL, alpha);
            blend_f(dst.fast_get<seze::RGB24>(x+i, y), COL, alpha);
          }
        } // for ilen
        // diagonal beams
        if (config::no_d)
          return false;
        auto ilen_half = ilen / 2;
        auto flen2_half = flen2 / 2.0f;
        if (ilen_half < 1 or flen2_half <= 0.0f)
          return false;
        for (int i = 1; i < ilen_half; ++i) {
          float fi = i / flen2_half;
          auto dist_ratio = std::lerp(1.0f, 0.0f, fi);
          auto alpha = dist_ratio * power * config::power;
          alpha = std::min(1.0f, alpha);
          blend_f(dst.fast_get<seze::RGB24>(x-i, y-i), COL, alpha);
          blend_f(dst.fast_get<seze::RGB24>(x+i, y+i), COL, alpha);
          blend_f(dst.fast_get<seze::RGB24>(x+i, y-i), COL, alpha);
          blend_f(dst.fast_get<seze::RGB24>(x-i, y+i), COL, alpha);
        } // for ilen
      } // if val > thrsld
      return true;
    }; // lambda
    if (use_red) func(value_r, blend_r, col.R);
    if (use_green) func(value_g, blend_g, col.G);
    if (use_blue) func(value_b, blend_b, col.B);
  } // for y/x
} // stars_rgb

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  seze::Image dst_pic(dst, mx, my, color_type);
  switch (config::mode) {
    case star_mode_t::rgb: stars_rgb(dst_pic, 1, 1, 1); break;
    case star_mode_t::red: stars_rgb(dst_pic, 1, 0, 0); break;
    case star_mode_t::green: stars_rgb(dst_pic, 0, 1, 0); break;
    case star_mode_t::blue: stars_rgb(dst_pic, 0, 0, 1); break;
    case star_mode_t::max3: stars_full<get_max3>(dst_pic); break;
    case star_mode_t::average:
    default: stars_full<get_average>(dst_pic); break;
  }
} // core

void finalize() {}
