#include <cstring>
#include <mutex>
#include <iostream>
extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include "utils/str.hpp"

using namespace seze;

static int line {0};
static int speed {3};
static std::vector<byte> buffer {};
static std::once_flag buffer_init {};
static bool random_speed {false};

void parse_opts(CP(char) options) {
  CmdParser parser(options);
  Str str;
// color finder
  str = parser.get_options({"-s", "--speed"});
  if ( !str.empty())
    speed = std::max(std::stoi(str), 1);
  str = parser.get_options({"-r", "--random"});
  if ( !str.empty())
    random_speed = true;
} // parse_opts

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Emulator for old pc graphic (MSX, ZX Spectrum)";
  info.info = "Usage:\n"
    "-s, --speed\tspeed for scanline\n"
    "-r, --random\tif defined, use random speed\n"
    ;
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  parse_opts(options);
  return info;
} // init

void warpscan(byte* dst, int stride, int my) {
  std::call_once(buffer_init, [stride, my]()
    { buffer.resize(stride * my); });
  auto speed_local {irand(1, speed)};
  FOR (i, speed_local) {
    auto dst_p {dst + stride * line};
    const auto buf_p {buffer.data() + stride * line};
    memcpy(buf_p, dst_p, stride);
    if (++line >= my)
        line = 0;
  }
  memcpy(dst, buffer.data(), stride * my);
}

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: p_dst is null");
  warpscan(dst, stride, my);
} // core

void finalize() {}
