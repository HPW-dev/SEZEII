extern "C" {
#include "plugin-api.h"
}
#include "utils/error.hpp"
#include "utils/cmd-parser.hpp"
#include "image/rgb24.hpp"
#include "image/image.hpp"
#include <cstring>
#include <functional>
#include <map>

using namespace seze;

enum class direction_t { horizontal, vertical };
enum class sort_t { max3, average, red, green, blue };

using color_op_t = std::function<byte (CN(RGB24))>;
byte average(CN(RGB24) c) { return (c.R + c.G + c.B) / 3; }
byte max3(CN(RGB24) c) { return std::max(c.R, std::max(c.G, c.B)); }
byte only_red(CN(RGB24) c) { return c.R; }
byte only_green(CN(RGB24) c) { return c.G; }
byte only_blue(CN(RGB24) c) { return c.B; }
color_op_t color_op = max3;

namespace config {
  bool fast_ver = false;
  bool bidirect = true;
  direction_t direction = direction_t::horizontal;
  sort_t sort_type = sort_t::max3;
  byte threashold = 220;
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Pixel sorter";
  info.info = "usage:\n"
    "-v, --threshold <0..255>\tthresholding value\n"
    "-d, --dir <DIR>\tsorting direction\n"
    "-t, --type <type_>\ttype of sorting\n"
    "-b, --bidir\tbidirectional sorting\n"
    "-f, --fast\tuse fast alg.\n"
    "avaliable directions: h, v\n"
    "avaliable sorting types: average, max3, red, green, blue\n"
    "example: -v 220 -d h -t max3\n";
// parse opts:
  CmdParser parser(options);
// --threshold:
  std::string str_threshold = parser.get_options({"-v", "--threshold"});
  if ( !str_threshold.empty()) {
    config::threashold = std::clamp<byte>(
      std::stoi(str_threshold), 0, 255);
  }
// --dir:
  std::string str_dir = parser.get_options({"-d", "--dir"});
  config::direction = str_dir == "v" ? direction_t::vertical
    : direction_t::horizontal;
// --type:
  std::map<std::string, sort_t> table_types = {
    {"max3", sort_t::max3},
    {"average", sort_t::average},
    {"red", sort_t::red},
    {"green", sort_t::green},
    {"blue", sort_t::blue},
  };
  std::string str_type = parser.get_options({"-t", "--type"});
  if ( !str_type.empty() ) {
    try {
      config::sort_type = table_types.at(str_type);
    } catch (...) {
      error("plugin init: unknown sort type");
    }
  }
  std::map<sort_t, color_op_t> color_op_map = {
    {sort_t::average, average},
    {sort_t::max3, max3},
    {sort_t::red, only_red},
    {sort_t::green, only_green},
    {sort_t::blue, only_blue},
  };
  color_op = color_op_map.at(config::sort_type);
// --bidir:
  if (parser.opt_exists("-b") or parser.opt_exists("--bidir"))
    config::bidirect = true;
  else
    config::bidirect = false;
// --fast
  if (parser.opt_exists("-f") or parser.opt_exists("--fast"))
    config::fast_ver = true;
  else
    config::fast_ver = false;
  return info;
} // init

static void fast_sort(byte* dst, int my, int stride) {
  cfor (y, my) {
    int count = 0;
    std::sort(dst + stride * y, dst + stride * (y + 1));
  } // for y
} // fast_sort

static bool is_dark(CN(RGB24) c)
{ return color_op(c) < config::threashold; }

static bool is_light(CN(RGB24) c)
{ return color_op(c) >= config::threashold; }

static void pixelsort_h(byte* dst, int mx, int my, color_t color_type) {
  seze::Image pic(dst, mx, my, color_type);
  auto sort_dir1 = [](CN(RGB24) a, CN(RGB24) b)->bool
    { return color_op(a) < color_op(b); };
  auto sort_dir2 = [](CN(RGB24) a, CN(RGB24) b)->bool
    { return color_op(a) > color_op(b); };
  auto f_sort = config::bidirect ? sort_dir2 : sort_dir1;
  cfor (y, my) {
    auto line = &pic.fast_get<RGB24>(0, y);
    bool find_dark = is_light(line[0]);
    int x_start = 0;
    int x_end = -1;
    for (int x = 1; x < mx; ++x) {
      // find x start
      if (x_start == -1) {
        auto& c = pic.fast_get<RGB24>(x, y);
        if (find_dark) {
          if (is_dark(c)) {
            x_start = x;
            find_dark = false;
            continue; // skit this x
          }
        } else {
          if (is_light(c)) {
            x_start = x;
            find_dark = true;
            continue; // skit this x
          }
        }
      }
      // find x end
      if (x_end == -1) {
        auto& c = pic.fast_get<RGB24>(x, y);
        if (find_dark) {
          if (is_dark(c)) {
            x_end = x;
            find_dark = false;
          }
        } else {
          if (is_light(c)) {
            x_end = x;
            find_dark = true;
          }
        }
      }
      // sort
      if (x_start != -1 and x_end != -1) {
        std::sort(line + x_start, line + x_end, f_sort);
        x_start = x_end = -1;
      }
    } // for x
  } // for y
} // pixelsort_h

void pixelsort_v(byte* dst, int mx, int my, color_t color_type) {
  seze::Image pic(dst, mx, my, color_type);
  auto sort_dir1 = [](CN(RGB24) a, CN(RGB24) b)->bool
    { return color_op(a) < color_op(b); };
  auto sort_dir2 = [](CN(RGB24) a, CN(RGB24) b)->bool
    { return color_op(a) > color_op(b); };
  auto f_sort = config::bidirect ? sort_dir2 : sort_dir1;
  cfor (x, mx) {
    std::vector<RGB24> vertical_buffer;
    vertical_buffer.reserve(my);
    bool find_dark = is_light(pic.fast_get<RGB24>(x, 0));
    int y_start = 0;
    int y_end = -1;
    for (int y = 1; y < my; ++y) {
      // find x start
      if (y_start == -1) {
        auto& c = pic.fast_get<RGB24>(x, y);
        if (find_dark) {
          if (is_dark(c)) {
            y_start = y;
            find_dark = false;
            continue; // skit this y
          }
        } else {
          if (is_light(c)) {
            y_start = y;
            find_dark = true;
            continue; // skit this y
          }
        }
      }
      // find y end
      if (y_end == -1) {
        auto& c = pic.fast_get<RGB24>(x, y);
        if (find_dark) {
          if (is_dark(c)) {
            y_end = y;
            find_dark = false;
          }
        } else {
          if (is_light(c)) {
            y_end = y;
            find_dark = true;
          }
        }
      }
      // sort
      if (y_start != -1 and y_end != -1) {
        for (int iy = y_start; iy < y_end; ++iy)
          vertical_buffer.push_back(pic.fast_get<RGB24>(x, iy));
        std::sort(vertical_buffer.begin(), vertical_buffer.end(), f_sort);
        for (int iy = y_start; iy < y_end; ++iy)
          pic.fast_set<RGB24>(x, iy, vertical_buffer[iy - y_start]);
        y_start = y_end = -1;
      }
    } // for x
  } // for y
} // pixelsort_v

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  if (config::fast_ver) {
    fast_sort(dst, my, stride);
    return;
  }
  if (config::direction == direction_t::horizontal)
    pixelsort_h(dst, mx, my, color_type);
  else
    pixelsort_v(dst, mx, my, color_type);
} // core

void finalize() {}
