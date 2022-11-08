#include <omp.h>
#include <cstring>
#include <atomic>
#include <map>
#include <mutex>
extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
//#include "utils/log.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include "image/rgb24.hpp"
#include "image/image.hpp"

using namespace seze;

enum class mode_e { average=0, xor_, or_, and_, sub, add, div, mul, min, max};

using ft_bitop = void (*)(byte&, CN(byte));

void bitop_xor(byte& y, CN(byte) x) { y ^= x; }
void bitop_or(byte& y, CN(byte) x) { y |= x; }
void bitop_and(byte& y, CN(byte) x) { y &= x; }
void bitop_sub(byte& y, CN(byte) x) { y -= x; }
void bitop_add(byte& y, CN(byte) x) { y += x; }
void bitop_div(byte& y, CN(byte) x) { y = x != 0 ? y / x : 0; }
void bitop_mul(byte& y, CN(byte) x) { y *= x; }
void bitop_min(byte& y, CN(byte) x) { y = std::min(x, y); }
void bitop_max(byte& y, CN(byte) x) { y = std::max(x, y); }

namespace config {
  uint count = 6; ///< count of frames 4 avr
  bool glitch_mode = false;
  mode_e mode = mode_e::average;
  ft_bitop bitop = nullptr;
}

namespace {
  std::vector<shared_p<Image>> frames = {};
  std::atomic_int current_frame = 0;
  std::mutex mu = {};
  inline std::once_flag frame_init_once_flag = {};
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Frame average effect";
  info.info = "usage:\n"
    "-o, --op\tpixel operation [default average]\n"
    "-c, --count\tcount of frames used for effect\n"
    "-g, --glitch\tglitch mode\n"
    "avaliable pixel operations: average, xor, or, and, add, sub, "
    "div, mul, min, max";
// parse opts:
  CmdParser parser(options);
  // --op
  std::string op_str = parser.get_options({"-o", "--op"});
  static std::map<std::string, std::pair<ft_bitop, mode_e>>
  op_table = {
    {"average", {nullptr, mode_e::average}},
    {"xor", {bitop_xor, mode_e::xor_}},
    {"and", {bitop_and, mode_e::and_}},
    {"or", {bitop_or, mode_e::or_}},
    {"add", {bitop_add, mode_e::add}},
    {"sub", {bitop_sub, mode_e::sub}},
    {"mul", {bitop_mul, mode_e::mul}},
    {"min", {bitop_min, mode_e::min}},
    {"max", {bitop_max, mode_e::max}},
  };
  if ( !op_str.empty()) {
    auto pair = op_table.at(op_str);
    config::bitop = pair.first;
    config::mode = pair.second;
  }
  // --count
  if (auto str = parser.get_options({"-c", "--count"}); !str.empty())
    config::count = std::max(1, std::stoi(str));
  // --glitch
  config::glitch_mode = (parser.opt_exists("-g")
    || parser.opt_exists("--glitch"));
  bit_set_if(&info.flags, PLGNINF_MULTITHREAD, config::glitch_mode);
  return info;
} // init

static void init_frames(CN(seze::Image) src) {
  std::lock_guard<decltype(mu)> lock(mu);
  FOR (i, config::count) {
    auto pic = make_shared_p<seze::Image>(src);
    frames.push_back(pic);
  }
}

static void frame_push(CN(seze::Image) src) {
  src.fast_copy_to(*frames[current_frame]);
  current_frame = (++current_frame) % config::count;
}

void average_process(seze::Image &dst) {
  #pragma omp parallel for
  FOR (i, dst.size) {
    uint R = 0, G = 0, B = 0;
    for (CN(auto) frame: frames) {
      auto c = frame->fast_get<RGB24>(i);
      R += c.R;
      G += c.G;
      B += c.B;
    }
    dst.fast_set(i, RGB24(
      R / config::count,
      G / config::count,
      B / config::count) );
  }
} // average_process

static void average_to_frame(seze::Image &dst) {
  if (config::mode == mode_e::average) {
    average_process(dst);
  } else { // other bitop's
    FOR (i, dst.size) {
      auto& dc = dst.fast_get<RGB24>(i);
      byte &R = dc.R, &G = dc.G, &B = dc.B;
      for (CN(auto) frame: frames) {
        auto c = frame->fast_get<RGB24>(i);
        config::bitop(R, c.R);
        config::bitop(G, c.G);
        config::bitop(B, c.B);
      }
    }
  }
} // average_to_frame

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  seze::Image dst_pic(dst, mx, my, color_type);
  std::call_once(frame_init_once_flag, &init_frames, dst_pic);
  frame_push(dst_pic);
  average_to_frame(dst_pic);
} // core

void finalize() {
  frames.clear();
}
