#include <map>
#include <atomic>
extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/types.hpp"
#include "utils/log.hpp"
#include "utils/error.hpp"
#include "utils/macro.hpp"
#include "utils/str.hpp"
#include "utils/cmd-parser.hpp"

using namespace seze;

static std::atomic_int blink_state = 0;
using Color = RGB24;
void (*process_f)(Image&) = {};
using process_ft = decltype(process_f);

enum class palette_name {
  none=0,
  BWRGB, // black, white, red, green, blue
  x3bit, // 8 colors
  x216, // 216 colors
  x36, // 36 colors
};

struct Palette {
  std::vector<Color> v_color = {};
  /** Смещает цвет каждый кадр для мерция.
  Вычисляется по палитре */
  Real gigascreen_offset = 0;
  palette_name name = palette_name::none;
};

static Palette palette = {};

void select_palette(palette_name name) {
  switch (name) {
    default:
    case palette_name::BWRGB: {
      /* результирующие цвета
      palette.v_color.push_back(Color(0x00,0x00,0x00));
      palette.v_color.push_back(Color(0xFF,0x00,0x00));
      palette.v_color.push_back(Color(0x00,0xFF,0x00));
      palette.v_color.push_back(Color(0x00,0x00,0xFF));
      palette.v_color.push_back(Color(0xFF,0xFF,0xFF));
      */
      palette.v_color.push_back(Color(0x00,0x00,0x00));

      palette.v_color.push_back(Color(0xFF,0x00,0x00));
      palette.v_color.push_back(Color(0xFF,0xFF,0x00));
      palette.v_color.push_back(Color(0x00,0xFF,0x00));
      palette.v_color.push_back(Color(0x00,0xFF,0xFF));
      palette.v_color.push_back(Color(0x00,0x00,0xFF));
      palette.v_color.push_back(Color(0xFF,0x00,0xFF));

      palette.v_color.push_back(Color(0x7F,0x00,0x00));
      palette.v_color.push_back(Color(0x00,0x7F,0x00));
      palette.v_color.push_back(Color(0x00,0x00,0x7F));

      palette.v_color.push_back(Color(0xFF,0x7F,0x7F));
      palette.v_color.push_back(Color(0x7F,0xFF,0x7F));
      palette.v_color.push_back(Color(0x7F,0x7F,0xFF));

      palette.v_color.push_back(Color(0xFF,0xFF,0xFF));
      palette.gigascreen_offset = 255.0 / 3.0;
      palette.name = palette_name::BWRGB;
      break;
    }
    case palette_name::x3bit: {
      palette.v_color.push_back(Color(0x00,0x00,0x00));
      palette.v_color.push_back(Color(0xFF,0x00,0x00));
      palette.v_color.push_back(Color(0xFF,0xFF,0x00));
      palette.v_color.push_back(Color(0x00,0xFF,0x00));
      palette.v_color.push_back(Color(0x00,0xFF,0xFF));
      palette.v_color.push_back(Color(0x00,0x00,0xFF));
      palette.v_color.push_back(Color(0xFF,0x00,0xFF));
      palette.v_color.push_back(Color(0xFF,0xFF,0xFF));
      palette.gigascreen_offset = 255.0 / 4.0;
      palette.name = palette_name::x3bit;
      break;
    }
    case palette_name::x216: {
      uint32_t u32_col = 0;
      FOR (r, 6)
      FOR (g, 6)
      FOR (b, 6)
        palette.v_color.push_back(Color(
          r * (255.0/5.0),
          g * (255.0/5.0),
          b * (255.0/5.0)
        ));
      palette.gigascreen_offset = 255.0 / 25.0;
      palette.name = palette_name::x216;
      break;
    }
    case palette_name::x36: {
      uint32_t u32_col = 0;
      FOR (r, 3)
      FOR (g, 4)
      FOR (b, 3)
        palette.v_color.push_back(Color(
          r * (255.0/2.0),
          g * (255.0/3.0),
          b * (255.0/2.0)
        ));
      palette.gigascreen_offset = 255.0 / 11.0;
      palette.name = palette_name::x216;
      break;
    }
  } // switch name
} // select_palette

template <typename T>
T pow2(T x) { return x*x; }

void palettize(Color &col, int offset) {
  int best_color = 0; // index for best color
  int i_min = 255 * 255 * 3;
  FOR (i, palette.v_color.size()) {
    auto pal_col = palette.v_color[i];
    int i_cur =
      pow2(pal_col.R - col.R + offset) +
      pow2(pal_col.G - col.G + offset) +
      pow2(pal_col.B - col.B + offset);
    if (i_cur < i_min) {
      best_color = i;
      i_min = i_cur;
    }
  } // for max pal
  col = palette.v_color[best_color];
} // palettize

void palettize_BWRGB(Color &col, int state) {
  int best_color = 0; // index for best color
  int i_min = 255 * 255 * 3;
  FOR (i, palette.v_color.size()) {
    auto pal_col = palette.v_color[i];
    int i_cur =
      pow2(pal_col.R - col.R) +
      pow2(pal_col.G - col.G) +
      pow2(pal_col.B - col.B);
    if (i_cur < i_min) {
      best_color = i;
      i_min = i_cur;
    }
  } // for max pal
  col = palette.v_color[best_color];
  if (col == Color(0xFF,0xFF,0x00))
    col = state ? Color(0xFF,0x00,0x00) : Color(0x00,0xFF,0x00);
  if (col == Color(0x00,0xFF,0xFF))
    col = state ? Color(0x00,0x00,0xFF) : Color(0x00,0xFF,0x00);
  if (col == Color(0xFF,0x00,0xFF))
    col = state ? Color(0x00,0x00,0xFF) : Color(0xFF,0x00,0x00);

  if (col == Color(0x7F,0x00,0x00))
    col = state ? Color(0x00,0x00,0x00) : Color(0xFF,0x00,0x00);
  if (col == Color(0x00,0x7F,0x00))
    col = state ? Color(0x00,0x00,0x00) : Color(0x00,0xFF,0x00);
  if (col == Color(0x00,0x00,0x7F))
    col = state ? Color(0x00,0x00,0x00) : Color(0x00,0x00,0xFF);

  if (col == Color(0xFF,0x7F,0x7F))
    col = state ? Color(0xFF,0xFF,0xFF) : Color(0xFF,0x00,0x00);
  if (col == Color(0x7F,0x7F,0xFF))
    col = state ? Color(0xFF,0xFF,0xFF) : Color(0x00,0xFF,0x00);
  if (col == Color(0x7F,0xFF,0x7F))
    col = state ? Color(0xFF,0xFF,0xFF) : Color(0x00,0x00,0xFF);
} // palettize_BWRGB

void gigascreen(Image& dst) {
  auto state = ++blink_state & 1;
  auto offset = state ?
    palette.gigascreen_offset :
    -palette.gigascreen_offset;
  if (palette.name == palette_name::BWRGB) {
    FOR (i, dst.size) {
      auto &col = dst.fast_get<Color>(i);
      palettize_BWRGB(col, state);
    }
    return;
  }
  FOR (i, dst.size) {
    auto &col = dst.fast_get<Color>(i);
    palettize(col, offset);
  }
}

//! RG- R-B -GB
void tricolor_2(Image& dst) {
  auto state = ++blink_state % 3;
  switch (state) {
    case 0: FOR (i, dst.size) dst.fast_get<Color>(i).R = 0; break;
    case 1: FOR (i, dst.size) dst.fast_get<Color>(i).G = 0; break;
    default: FOR (i, dst.size) dst.fast_get<Color>(i).B = 0; break;
  }
} // tricolor_2

//! R-- -G- --B
void tricolor(Image& dst) {
  auto state = ++blink_state % 3;
  switch (state) {
    case 0: FOR (i, dst.size) { auto &col = dst.fast_get<Color>(i); col.G = 0; col.B = 0; } break;
    case 1: FOR (i, dst.size) { auto &col = dst.fast_get<Color>(i); col.R = 0; col.B = 0; } break;
    default: FOR (i, dst.size) { auto &col = dst.fast_get<Color>(i); col.R = 0; col.G = 0; } break;
  }
} // tricolor

void parse(CN(Str) options) {
  auto pal_name = palette_name::BWRGB;
  process_f = &gigascreen;
  CmdParser parser(options);
  // select palette
  if (auto str = parser.get_options({"-p", "--palette"}); !str.empty()) {
    std::map<Str, palette_name> table = {
      {"bwrgb", palette_name::BWRGB},
      {"216", palette_name::x216},
      {"36", palette_name::x36},
      {"3bit", palette_name::x3bit},
    };
    try {
      pal_name = table.at(str);
    } catch (...) {
      error("init: bad palette name");
    }
  }
  // select mode
  if (auto str = parser.get_options({"-m", "--mode"}); !str.empty()) {
    std::map<Str, process_ft> table = {
      {"gigascreen", &gigascreen},
      {"tricolor", &tricolor},
      {"tricolor2", &tricolor_2},
    };
    try {
      process_f = table.at(str);
    } catch (...) {
      error("init: star draw mode mapping error");
    }
  }
  select_palette(pal_name);
} // parse

PluginInfo init(CP(char) options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Gigascreen effect";
  info.info = "Usage:\n"
    "-m, --mode\tgigascreen, tricolor, tricolor2\n"
    "-p, --palette\tbwrgb, 3bit, 216, 36\n";
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  parse(options);
  return info;
} // init

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  Image dst_pic(dst, mx, my, color_type);
  process_f(dst_pic);
} // core

void finalize() {}
