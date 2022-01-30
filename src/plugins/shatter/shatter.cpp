extern "C" {
#include "../../../plugin-api.h"
//#include "image/image.hpp"
}

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Shaky motion blur";
  return info;
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  /*int bytes = my * stride;
  for (int i = 0; i < bytes; ++i)
    dst[i] = ~dst[i];*/
  //seze::Image dst_pic(dst, mx, my, color_type);
}

void finalize() {}
