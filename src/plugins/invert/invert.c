#include "../../../plugin-api.h"

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "color inversion";
  return info;
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  int bytes = my * stride;
  for (int i = 0; i < bytes; ++i)
    dst[i] = ~dst[i];
}

void finalize() {}
