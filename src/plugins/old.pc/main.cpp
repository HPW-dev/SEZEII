extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include "image/rgb24.hpp"
#include "image/image.hpp"

using namespace seze;

void parse_opts(CP(char) options) { 
// parse opts:
  /*CmdParser parser(options);
  // --op
  std::string op_str = parser.get_options({"-o", "--op"});
  static std::map<std::string, std::pair<ft_bitop, mode_e>>
  op_table = {
    {"average", {nullptr, mode_e::average}},
    {"xor", {bitop_xor, mode_e::xor_}},
    {"and", {bitop_and, mode_e::and_}},
    {"or", {bitop_or, mode_e::or_}},
    {"add", {bitop_add, mode_e::add}},
    {"sub", {bitop_sub, mode_e::sub}},
    {"mul", {bitop_mul, mode_e::mul}},
    {"min", {bitop_min, mode_e::min}},
    {"max", {bitop_max, mode_e::max}},
  };
  if ( !op_str.empty()) {
    auto pair = op_table.at(op_str);
    config::bitop = pair.first;
    config::mode = pair.second;
  }
  // --count
  if (auto str = parser.get_options({"-c", "--count"}); !str.empty())
    config::count = std::max(1, std::stoi(str));
  // --glitch
  config::glitch_mode = (parser.opt_exists("-g")
    || parser.opt_exists("--glitch"));*/
}

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Frame average effect";
  info.info = "usage:\n";
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  parse_opts(options);
  return info;
} // init

void core(byte* p_dst, int mx, int my, int stride, color_t color_type) {
  iferror( !p_dst, "core: p_dst is null");
  iferror( color_type != color_t::seze_RGB24,
    "core: color_type is not RGB24");
  seze::Image dst_pic(p_dst, mx, my, color_type);
} // core

void finalize() {}
