extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/error.hpp"
#include <map>

enum style_e { avg, mid, max, min, end, begin };

namespace {
  struct config_t {
    uint len = 10;
    bool random_len = false;
    style_e style = style_e::avg;
  } config = {};
}

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Shaky motion blur";
  info.info = "Usage:\n"
    "-l, --len\t\tlength of motion [default: 10]\n"
    "-rl, --random-len\trandomize motion length in interval of -l value\n"
    "-s, --style\t\tstyle of blending: avg, mid, max,"
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
      error("init: tv_type mapping error");
    }
  } // -s --style
  return info;
} // init

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  seze::Image dst_pic(dst, mx, my, color_type);
}

void finalize() {}
