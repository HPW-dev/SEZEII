#include <map>
extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/log.hpp"
#include "utils/cmd-parser.hpp"

using namespace seze;

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Gigascreen effect";
  info.info = "Usage:\n";
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  //seze::Image dst_pic(dst, mx, my, color_type);
} // core

void finalize() {}
