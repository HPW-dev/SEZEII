#include <mutex>
extern "C" {
#include "plugin-api.h"
}
#include "../tvsim2bw.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

Tvsim2bw tvsim2bw {};
seze::Image dst_pic {};
seze::Image buffer {};
std::once_flag call_once_flag {};

struct PluginInfo init(CP(char) options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "TVsim 2 (BW)";
  info.info = "USAGE: enter empty --opts for see all commands";
  bit_set_if(&info.flags, PLGNINF_MULTITHREAD, 0);
  opts_to_conf(Str(options), tvsim2bw.conf);
  return info;
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  std::call_once(call_once_flag, [&] {
    dst_pic.init(dst, mx, my, color_type);
    buffer.init(mx, my, color_type);
  });
  std::copy(dst, dst + stride * my, buffer.get_data());
  tvsim2bw(buffer, dst_pic);
}

void finalize() {}
