#include <fstream>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "plugin-api.h"
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/log.hpp"
//#include "utils/random.hpp"

namespace config {
  inline float brightness = 0;
  inline float contrast = 1;
  inline std::unordered_map<byte, seze::RGB24> table {}; /// gradient map
}

void init_palette(CN(Str) palette_path) {
  int w, h, comp;
  auto data = stbi_load(palette_path.c_str(), &w, &h, &comp, STBI_rgb);

  iferror(w != 256, "palette image width != 256");
  iferror(h < 1, "palette image height < 1");
  iferror(comp != 3, "palette image comp != 3");
  iferror( !data, "palette file not opened");

  static_assert( sizeof(seze::RGB24) == 3 );
  cfor (idx, 256)
    config::table[idx] = rcast(seze::RGB24*, data)[idx];
} // init_palette

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Gradient map";
  info.info = "Usage:\n"
    "-c, --contrast\t\timage contrast\n"
    "-b, --brightness\timage brightness\n"
    "-p, --palette\t\tpalette file (.png)\n";

// parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_options({"-c", "--contrast"}); !str.empty())
    config::contrast = std::clamp<float>(std::stof(str), 0, 20);
  if (auto str = parser.get_options({"-b", "--brightness"}); !str.empty())
    config::brightness = std::clamp<float>(std::stof(str), -1, 1);
  if (auto str = parser.get_options({"-p", "--palette"}); !str.empty()) {
    init_palette(str);
  } else {
    error("need -p parameter (see usage)\n");
  }

  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

void process(seze::Image& dst) {
  assert( !config::table.empty());

  cfor (i, dst.size) {
    nauto pixel = dst.fast_get<seze::RGB24>(i);
    // TODO выбор режима обесцвечивания
    double luma =
      (pixel.R * config::contrast + config::brightness * 255.0) * 0.2627 +
      (pixel.B * config::contrast + config::brightness * 255.0) * 0.6780 +
      (pixel.G * config::contrast + config::brightness * 255.0) * 0.0593;
    luma = std::clamp<double>(luma, 0, 255);
    pixel = config::table.at( scast(uint, luma) );
  }
} // process

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  assert(dst);
  assert(color_type == color_t::seze_RGB24);
  seze::Image dst_pic(dst, mx, my, color_type);
  process(dst_pic);
}

void finalize() {}
