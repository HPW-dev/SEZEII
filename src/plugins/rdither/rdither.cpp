extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"

using namespace seze;

namespace config {
  float contrast = 0;
  int brightness = 0;
}

static void apply_brightness(int& dst, int val) { dst += val; }

static void apply_contrast(int& dst, float val) {
  auto factor = (259.0f * (val + 255.0f)) / (255.0f * (259.0f - val));
  dst = factor * (dst - 127.0f) + 127.0f;
}

static void prepare_color(int& dst) {
  apply_contrast(dst, config::contrast);
  apply_brightness(dst, config::brightness);
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "fast random dithering";
  info.info = "Usage:\n"
    "-c\tcontrast [-255..255]\n"
    "-b\tbrightness [-255..255]\n";
// parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_option("-c"); !str.empty())
    config::contrast = std::stof(str);
  if (auto str = parser.get_option("-b"); !str.empty())
    config::brightness = std::stoi(str);
  seze::randomize_seed();
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  seze::Image dst_pic(dst, mx, my, color_type);
  seze::Image rgbi_pic(mx, my, seze_RGBi);
// copy dst to RGBi
  cfor (i, dst_pic.size) {
    auto c = dst_pic.fast_get<seze::RGB24>(i);
    rgbi_pic.fast_set<seze::RGBi>(i, seze::RGBi(c.R, c.G, c.B));
  }
// dithering:
  auto ptr = rcast(int*, rgbi_pic.get_data());
  auto end = rcast(int*, rgbi_pic.get_data() + rgbi_pic.bytes);
  while (ptr < end) {
    prepare_color(*ptr);
    *ptr = (*ptr > (seze::irand_fast() % 255)) ? 255 : 0;
    ++ptr;
  }
// copy dst to RGBi
  cfor (i, rgbi_pic.size) {
    auto c = rgbi_pic.fast_get<seze::RGBi>(i);
    auto r = std::clamp(c.R, 0, 255);
    auto g = std::clamp(c.G, 0, 255);
    auto b = std::clamp(c.B, 0, 255);
    dst_pic.fast_set<seze::RGB24>(i, seze::RGB24(r, g, b));
  }
} // core

void finalize() {}
