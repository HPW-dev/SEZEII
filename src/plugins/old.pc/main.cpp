extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "utils/random.hpp"
#include "image/rgb24.hpp"
#include "image/image.hpp"
#include "palette.hpp"
#include "old-pc.hpp"
#include "color.finder/minmax.hpp"
#include "color.finder/most-common.hpp"
#include "color.selector/diff.hpp"
#include "palette.accepter/diff.hpp"
#include "palette.accepter/empty.hpp"
#include "dither/bayer-2x2.hpp"
#include "dither/bayer-16x16.hpp"

using namespace seze;

static shared_p<Old_pc> old_pc {};
static shared_p<Color_finder> color_finder {};
static shared_p<Color_selector> сolor_selector {};
static shared_p<Palette_accepter> palette_accepter {};
static shared_p<Dither> dither {};

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
  info.title = "Emulator for old pc graphic (MSX, ZX Spectrum)";
  info.info = "Usage:\n"
    "-f, --color-find\talg/ for find two colors in color block\n"
    //#"-s, --color-select\talg/ for select result color in two finded colors\n"
    "-a, --palette-alg\talg. for accept color from palette\n"
    "-d, --dither\t\tdithering alg.\n"
    "-v, --dither-value\tpower of dither effect\n"
    "-p, --palette\t\tpdn palette file (.txt with hex)\n"
    "-w, --width\t\twidth of color block\n"
    "-h, --height\t\theight of color block\n"
    "Avaliable dithers: 2x2, 16x16\n"
    "Avaliable color find algs: mc, diff\n"
    "Avaliable palette accept algs: none, diff\n"
    "Example usage: -f diff -a diff -d 16x16 -v 2 -p \"path to palette.txt\" -w 16 -h 16\n"
    ;
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  parse_opts(options);

  Palette pal;
  //init_rgb1b(pal);
  load_pdn_pal(pal, "C:\\Users\\GANE standart\\Documents\\paint.net User Files\\Palettes\\ZeroRanger_GREEN_ORANGE.txt");
  color_finder = make_shared_p<Color_finder_minmax>();
  сolor_selector = make_shared_p<Color_selector_diff>();
  palette_accepter = make_shared_p<Palette_accepter_diff>();
  dither = make_shared_p<Dither_bayer_2x2>();
  // настройка эффекта Old_pc:
  old_pc = make_shared_p<Old_pc>();
  old_pc->set_pal(pal);
  old_pc->set_block_size(vec2u{16, 16}); // TODO
  old_pc->set_color_finder(color_finder.get());
  old_pc->set_color_selector(сolor_selector.get());
  old_pc->set_dither(dither.get(), 1);
  old_pc->set_palette_accepter(palette_accepter.get());

  return info;
} // init

void core(byte* p_dst, int mx, int my, int stride, color_t color_type) {
  iferror( !p_dst, "core: p_dst is null");
  iferror( color_type != color_t::seze_RGB24,
    "core: color_type is not RGB24");
  seze::Image dst_pic(p_dst, mx, my, color_type);
  (*old_pc)(dst_pic);
} // core

void finalize() {}
