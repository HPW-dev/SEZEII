#include "../plugin-info.hpp"
#include "../../utils/error.hpp"

using namespace seze;

extern "C" {
  PluginInfo init(CN(std::string) options);
  void core(byte* dst, int x, int y, int stride, color_t color_type);
  void finalize();
}

PluginInfo init(CN(std::string) options) {
  PluginInfo info;
  info.color_type = color_t::RGB24;
  info.title = "color inversion";
  return info;
}

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  auto bytes = my * stride;
  std::transform(dst, dst + bytes, dst,
    [](CN(byte) pix)->byte { return ~pix; });
}

void finalize() {}
