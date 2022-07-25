#include <map>
#include <iostream>
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
  CmdParser parser(options);
  Str str;
// color finder
  str = parser.get_options({"-f", "--color-find"});
  if (str.empty()) {
    color_finder = make_shared_p<Color_finder_minmax>();
  } else {
    std::map<Str, void (*)()> cf_table {
      {"minmax", []{ color_finder = make_shared_p<Color_finder_minmax>(); } },
      {"mc", []{ color_finder = make_shared_p<Color_finder_most_common>(); } },
    };
    try {
      auto exec {cf_table.at(str)};
      (*exec)();
    } catch (...) {
      error("core: need correct name for color finder");
    }
  }

// palette
  Palette pal;
  str = parser.get_options({"-p", "--palette"});
  if (str.empty())
    init_rgb1b(pal);
  else
    load_pdn_pal(pal, str);

// palette accepter
  str = parser.get_options({"-a", "--palette-alg"});
  if (str.empty()) {
    palette_accepter = make_shared_p<Palette_accepter_diff>();
  } else {
    std::map<Str, void (*)()> pal_table {
      {"diff", []{ palette_accepter = make_shared_p<Palette_accepter_diff>(); } },
      {"none", []{ palette_accepter = make_shared_p<Palette_accepter_empty>(); } },
    };
    try {
      auto exec {pal_table.at(str)};
      (*exec)();
    } catch (...) {
      error("core: need correct name for palette accept alg.");
    }
  }
  
// dither
  str = parser.get_options({"-d", "--dither"});
  if ( !str.empty()) {
    std::map<Str, void (*)()> dith_table {
      {"2x2", []{ dither = make_shared_p<Dither_bayer_2x2>(); } },
      {"16x16", []{ dither = make_shared_p<Dither_bayer_16x16>(); } },
    };
    try {
      auto exec {dith_table.at(str)};
      (*exec)();
    } catch (...) {
      error("core: need correct name dithering alg.");
    }
  }

  str = parser.get_options({"-w", "--width"});
  uint w;
  if (str.empty())
    w = 16;
  else
    w = std::stoul(str);

  str = parser.get_options({"-h", "--height"});
  uint h;
  if (str.empty())
    h = 16;
  else
    h = std::stoul(str);

  str = parser.get_options({"-v", "--dither-value"});
  Real dither_power;
  if (str.empty())
    dither_power = 1.0f;
  else
    dither_power = std::stof(str);

  // настройка эффекта Old_pc:
  old_pc = make_shared_p<Old_pc>();
  old_pc->set_pal(pal);
  old_pc->set_block_size(vec2u{w, h});
  old_pc->set_color_finder(color_finder.get());
  сolor_selector = make_shared_p<Color_selector_diff>();
  old_pc->set_color_selector(сolor_selector.get());
  old_pc->set_dither(dither.get(), dither_power);
  old_pc->set_palette_accepter(palette_accepter.get());
} // parse_opts

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
    "Avaliable color find algs: mc, minmax\n"
    "Avaliable palette accept algs: none, diff\n"
    "Example usage: -f minmax -a diff -d 16x16 -v 2 -p \"path to palette.txt\" -w 16 -h 16\n"
    ;
  bit_enable(&info.flags, PLGNINF_MULTITHREAD);
  parse_opts(options);
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
