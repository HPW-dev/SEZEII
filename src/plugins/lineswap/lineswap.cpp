extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
//#include "utils/log.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include <cstring>

using namespace seze;

namespace config {
  int shift_len = 1;
  int count = 8;
  bool copy_mode = true;
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Frame line-copyer/swapper";
  info.info = "usage:\n"
    "-s, --shift\tlength of line copying\n"
    "-c, --count\tcount of repeat effect\n"
    "--swap\tuse swap instead copy\n";
  seze::randomize_seed();
// parse opts:
  CmdParser parser(options);
  // --shift
  std::string shift_str = parser.get_options({"-s", "--shift"});
  if ( !shift_str.empty()) {
    config::shift_len = std::stoi(shift_str);
    config::shift_len = std::max(config::shift_len, 1);
  }
  // --count
  std::string count_str = parser.get_options({"-c", "--count"});
  if ( !count_str.empty()) {
    config::count = std::stoi(count_str);
    config::count = std::max(config::count, 1);
  }
  // --swap
  config::copy_mode = parser.opt_exists("--swap") ? false : true;
  return info;
} // init

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  if (config::copy_mode) {
    auto buffer_bytes = stride * my;
    auto buffer = new byte[buffer_bytes];
    FOR (_, config::count) {
      memcpy(buffer, dst, my * stride);
      FOR (y, my) {
        int rnd_y = std::clamp(seze::irand(-config::shift_len, config::shift_len) + y, 0, my - 1);
        if (rnd_y == y)
          continue;
        memcpy(&dst[y * stride], &buffer[rnd_y * stride], stride);
      } // for y
    } // for count
    delete[] buffer;
  } else { // swap mode
    FOR (_, config::count) {
      FOR (y, my) {
        int rnd_y = std::clamp(seze::irand(-config::shift_len, config::shift_len) + y, 0, my - 1);
        if (rnd_y == y)
          continue;
        FOR (i, stride)
          std::swap(dst[y * stride + i], dst[rnd_y * stride + i]);
      } // for y
    } // for count
  } // if copy_mode
} // core

void finalize() {}
