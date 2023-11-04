#include <cassert>
#include <memory>
#include <mutex>
#include "plugin-api.h"
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/log.hpp"
#include "utils/random.hpp"

namespace config {
  inline float fade_speed {0.01}; /// скорость затухания предыдущего кадра
}

static std::unique_ptr<seze::Image> dst_frame {};
static std::unique_ptr<seze::Image> prev_frame {};
static std::once_flag once_init_frames {};

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Simple VHS";
  info.info = "Usage:\n"
    /*"-c, --contrast\t\timage contrast\n"
    "-b, --brightness\timage brightness\n"
    "-p, --palette\t\tpalette file (.png)\n"*/;

// parse opts:
  CmdParser parser(options);
  /*if (auto str = parser.get_options({"-c", "--contrast"}); !str.empty())
    config::contrast = std::clamp<float>(std::stof(str), 0, 20);
  if (auto str = parser.get_options({"-b", "--brightness"}); !str.empty())
    config::brightness = std::clamp<float>(std::stof(str), -1, 1);
  if (auto str = parser.get_options({"-p", "--palette"}); !str.empty()) {
    init_palette(str);
  } else {
    error("need -p parameter (see usage)\n");
  }*/

  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  return info;
} // init

inline void accept_fade(seze::Image& dst) {
  cfor (i, dst.bytes)
    dst.get_data()[i] = std::max(dst.get_cdata()[i], prev_frame->get_cdata()[i]);
  // предыдущий кадр становится текущим с затуханием
  cfor (i, dst.bytes)
    prev_frame->get_data()[i] = std::max<int>(0, int(dst.get_cdata()[i]) - (255 * config::fade_speed));
}

void process(seze::Image& dst) {
  accept_fade(dst);
  /*assert( !config::table.empty());

  cfor (i, dst.size) {
    nauto pixel = dst.fast_get<seze::RGB24>(i);
    // TODO выбор режима обесцвечивания
    double luma =
      (pixel.R * config::contrast + config::brightness * 255.0) * 0.2627 +
      (pixel.B * config::contrast + config::brightness * 255.0) * 0.6780 +
      (pixel.G * config::contrast + config::brightness * 255.0) * 0.0593;
    luma = std::clamp<double>(luma, 0, 255);
    pixel = config::table.at( scast(uint, luma) );
  }*/
} // process

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  assert(dst);
  assert(color_type == color_t::seze_RGB24);

  auto init_frames = [dst, mx, my, color_type]{
    dst_frame = std::make_unique<seze::Image>(dst, mx, my, color_type);
    prev_frame = std::make_unique<seze::Image>(mx, my, color_type);
  };
  std::call_once(once_init_frames, init_frames);

  process(*dst_frame);
}

void finalize() {}
