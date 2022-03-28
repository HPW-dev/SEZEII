#include <omp.h>
#include <cstring>
#include <vector>
#include <numeric>
extern "C" {
#include "plugin-api.h"
}
#include "utils/log.hpp"
#include "utils/macro.hpp"
#include "utils/aalloc.hpp"
#include "utils/error.hpp"

struct PluginInfo init(CP(char) options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Video stabelyzer";
  info.version = 3;
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
}

void core(byte* dst, int /*mx*/, int my, int stride,
color_t /*color_type*/) {
  
} // core

void finalize() {
  
}
