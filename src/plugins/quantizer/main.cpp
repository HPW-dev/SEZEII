#include <map>
extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "utils/log.hpp"
#include "utils/cmd-parser.hpp"
#include "image/rgb24.hpp"
#include "image/image.hpp"
#include "quantizer.hpp"
#include "bw.hpp"
#include "rgb-1bit.hpp"
#include "rgb-2bit.hpp"
#include "rgb242.hpp"

using namespace seze;

static shared_p<Quantizer> quantizer {};

void parse_opts(CP(char) options) {
  CmdParser parser(options);
  // выбор палитры
  std::string pal_str {parser.get_options({"-p", "--palette"})};
  std::map<std::string, void (*)()> pal_table {
    {"bw", []{ quantizer = make_shared_p<BW_qnt>(); } },
    {"rgb1bit", []{ quantizer = make_shared_p<RGB_1bit_qnt>(); } },
    {"rgb2bit", []{ quantizer = make_shared_p<RGB_2bit_qnt>(); } },
    {"rgb242", []{ quantizer = make_shared_p<RGB242_qnt>(); } },
  };
  try {
    auto exec {pal_table.at(pal_str)};
    (*exec)();
  } catch (...) {
    error("need correct palette name");
  }
} // parse_opts

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Color quantization plugin";
  info.info = "Usage:\n"
    "-p, --palette\tpalette type name\n";
    "Available palette types: bw, rgb1bit, rgb2bit, rgb242";
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  parse_opts(options);
  return info;
} // init

void core(byte* dst_data, int mx, int my, int stride, color_t color_type) {
  iferror( !dst_data, "core: dst_data is null");
  iferror( color_type != color_t::seze_RGB24,
    "core: supported only RGB24 format");
  Image dst(dst_data, mx, my, color_type);
  (*quantizer)(dst);
} // core

void finalize() {}
