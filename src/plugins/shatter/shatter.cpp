extern "C" {
#include "plugin-api.h"
}
#include "image/image.hpp"

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "Shaky motion blur";
  /*info.info = "Usage:\n"
    "-l, --len\tlength of star beams\n"
    "-p, --power\tstar lightnes power\n"
    "-t, --threshold\tthreshold value of color [0..254]\n"
    "-r, --rspawn\tenable random star swapning\n"
    "-s, --spawn\tpercent of spawn star (use with -r)\n"
    "--rsize\t\tenable random star size\n"
    "-m, --mode\tdrawing mode. Avaliable: avr, max3, rgb, red, green, blue\n"
    "--nv\tdisable vertical beams\n"
    "--nh\tdisable horizontal beams\n"
    "--nd\tdisable diagonal beams\n";
  // parse opts:
  CmdParser parser(options);
  if (auto str = parser.get_options({"-l", "--len"}); !str.empty())
    config::len = std::stof(str);*/
  return info;
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  seze::Image dst_pic(dst, mx, my, color_type);
}

void finalize() {}
