extern "C" {
#include "../../../plugin-api.h"
}
#include "../../utils/cmd-parser.hpp"
#include "bw.hpp"
#include "tv-yuv.hpp"
#include <map>

using namespace seze;

namespace {
TVsim* simulator = nullptr;
}

PluginInfo init(const char* options) {
  PluginInfo info;
  PluginInfo_init(&info);
  std::string title = "Analog TV";
#ifdef TVSIM_LD_COMPONENT
  title += " (LD precission mode)";
#endif
  info.version = 2;
  info.title = title.c_str();
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  std::string str_info = "HPW-dev 15.11.2021\nUSAGE:\n"
  "--first_bound\t\tlength of front porch (default 35)\n"
  "--second_bound\t\tlength of back porch (default 21)\n"
  "--hsync_size\t\tlength of h-sync (default 65)\n"
  "--vsync_size\t\tlength of v-sync (default 6571)\n"
  "--vsync_detect\t\tcount of sync signals for start v-sync (default 100)\n"
  "--first_strings\t\tcount of first h-blank strings(default 120)\n"
  "--second_strings\tcount of last h-blank strings (default 80)\n"
  "--white_level\t\tlevel of white color (default 1)\n"
  "--black_level\t\tlevel of black color (default 0.1)\n"
  "--blank_level\t\tlevel of empty signal (default 0)\n"
  "--sync_level\t\tlevel of synchronization signal (default -0.4)\n"
  "--empty_level\t\tlevel for empty strings\n"
  "--noise_level\t\tnoise power for luma (default 0.02)\n"
  "--noise_level_u\t\tnoise power for U(Cb) channer (default 0.02)\n"
  "--noise_level_v\t\tnoise power for V(Cr) channer (default 0.02)\n"
  "--AM_shift\t\tshift of AM signal for luma (default 0)\n"
  "--AM_shift_u\t\tshift of AM signal for U(Cb) (default 0)\n"
  "--AM_shift_v\t\tshift of AM signal for V(Cr) (default 0)\n"
  "--AM_ratio_in\t\tinput AM freg. ratio for luma (default 0.01)\n"
  "--AM_ratio_in_u\t\tinput AM freg. ratio for luma (default 0.01)\n"
  "--AM_ratio_in_v\t\tinput AM freg. ratio for U(Cb) (default 0.01)\n"
  "--AM_ratio_out\t\toutput AM freg. ratio for V(Cr) (default 0.01)\n"
  "--AM_ratio_out_u\toutput AM freg. ratio for U(Cb) (default 0.01)\n"
  "--AM_ratio_out_v\toutput AM freg. ratio for V(Cr) (default 0.01)\n"
  "--beam_speed\t\th-speed of beam (default 1)\n"
  "--beam_reverse_speed\treverse h-speed of beam (default 10)\n"
  "--hbound\t\tbeam h-bound (default 4)\n"
  "--fading\t\tcolor fading (default 0.2)\n"
  "--power\t\t\tpower of luma signal (default 1.3)\n"
  "--power_u\t\tpower of U(Cb) signal (default 1)\n"
  "--power_v\t\tpower of V(Cr) signal (default 1)\n"
  //"--power_r\t (default 1)\n"
  //"--power_g\t (default 1)\n"
  //"--power_b\t (default 1)\n"
  "--signal_shift\t\tv-shift for luma signal (default 0.01)\n"
  "--signal_shift_u\tv-shift for U(Cb) signal (default 0)\n"
  "--signal_shift_v\tv-shift for V(Cr) signal (default 0)\n"
  "--interlace\t\tif defined, use interlacing\n"
  "--enable_AM\t\tif defined, use AM simulation\n"
  "--filter_type [name]\ttype of filtration\n"
  "--filter_power\t\tpower of luma filter (default 2)\n"
  "--filter_power_u\tpower of U(Cb) filter (default 5)\n"
  "--filter_power_v\tpower of V(Cr) filter (default 5)\n"
  "--tv_type [name]\ttype of TV simulation\n"
  "--resolution_type [name] standart resolution type\n"
  "--ringing_ratio\t\tringing effect ratio - cos(ratio * x) (default 1)\n"
  "--ringing_length\trirnging effect width (default 16)\n"
  "--ringing_power\t\tpower of ringing effect (default 0)\n"
  "--ringing_type\t\ttype of ringing effect (default right)\n"
  "avaliable filters:\taverage, fast, accurate\n" // TODO +median
  "avaliable tv types:\tcolor, bw\n" // TODO +VGA-RGB
  "avaliable resolutions:\tCD-NTSC, CD-PAL, DVD-NTSC, UMD, SVCD, NTSC, PAL, RAW\n"
  "avaliable ringing effects: right, bidir\n";
// parse opts:
  CmdParser parser(options);
#define SET_F_IF(parm, opt) if (auto str = parser.get_option(opt); !str.empty()) \
  parm = std::stold(str);
#define SET_I_IF(parm, opt) if (auto str = parser.get_option(opt); !str.empty()) \
  parm = std::stoi(str);
  SET_I_IF(tv::first_bound, "--first_bound");
  SET_I_IF(tv::second_bound, "--second_bound");
  SET_I_IF(tv::hsync_size, "--hsync_size");
  SET_I_IF(tv::vsync_size, "--vsync_size");
  SET_I_IF(tv::vsync_detect, "--vsync_detect");
  SET_I_IF(tv::first_strings, "--first_strings");
  SET_I_IF(tv::second_strings, "--second_strings");
  SET_F_IF(tv::white_level, "--white_level");
  SET_F_IF(tv::black_level, "--black_level");
  SET_F_IF(tv::blank_level, "--blank_level");
  SET_F_IF(tv::sync_level, "--sync_level");
  SET_F_IF(tv::empty_level, "--empty_level");
  SET_F_IF(tv::noise_level, "--noise_level");
  SET_F_IF(tv::noise_level_u, "--noise_level_u");
  SET_F_IF(tv::noise_level_v, "--noise_level_v");
  SET_F_IF(tv::AM_shift, "--AM_shift");
  SET_F_IF(tv::AM_shift_u, "--AM_shift_u");
  SET_F_IF(tv::AM_shift_v, "--AM_shift_v");
  SET_F_IF(tv::AM_ratio_in, "--AM_ratio_in");
  SET_F_IF(tv::AM_ratio_in_u, "--AM_ratio_in_u");
  SET_F_IF(tv::AM_ratio_in_v, "--AM_ratio_in_v");
  SET_F_IF(tv::AM_ratio_out, "--AM_ratio_out");
  SET_F_IF(tv::AM_ratio_out_u, "--AM_ratio_out_u");
  SET_F_IF(tv::AM_ratio_out_v, "--AM_ratio_out_v");
  SET_F_IF(tv::beam_speed, "--beam_speed");
  SET_F_IF(tv::beam_reverse_speed, "--beam_reverse_speed");
  SET_F_IF(tv::hbound, "--hbound");
  SET_F_IF(tv::fading, "--fading");
  SET_F_IF(tv::power, "--power");
  SET_F_IF(tv::power_u, "--power_u");
  SET_F_IF(tv::power_v, "--power_v");
  SET_F_IF(tv::signal_shift, "--signal_shift");
  SET_F_IF(tv::signal_shift_u, "--signal_shift_u");
  SET_F_IF(tv::signal_shift_v, "--signal_shift_v");
  tv::interlace = parser.opt_exists("--interlace") ? true : false;
  tv::enable_AM = parser.opt_exists("--enable_AM") ? true : false;
  SET_F_IF(tv::filter_power, "--filter_power");
  SET_F_IF(tv::filter_power_u, "--filter_power_u");
  SET_F_IF(tv::filter_power_v, "--filter_power_v");
  SET_I_IF(tv::ringing_length, "--ringing_length");
  SET_F_IF(tv::ringing_power, "--ringing_power");
  SET_F_IF(tv::ringing_ratio, "--ringing_ratio");
// select resolutions:
  auto resolution_type_str = parser.get_option("--resolution_type");
  if ( !resolution_type_str.empty()) {
    std::map<std::string, resolution_e> table = {
      {"CD-NTSC", resolution_e::CD_NTSC},
      {"CD-PAL", resolution_e::CD_PAL},
      {"DVD-NTSC", resolution_e::DVD_NTSC},
      {"UMD", resolution_e::UMD},
      {"SVCD", resolution_e::SVCD},
      {"NTSC", resolution_e::NTSC},
      {"PAL", resolution_e::PAL},
      {"RAW", resolution_e::RAW},
    };
    try {
      tv::resolution_type = table.at(resolution_type_str);
    } catch (...) {
      error("init: resolution name mapping error");
    }
  }
  resolution_pair resolution;
  try {
    resolution = table_resolution.at(tv::resolution_type);\
    if (resolution.x != -1) { // if not raw res
      info.in_x = resolution.x;
      info.in_y = resolution.y;
    }
  } catch(...) {
    error("init: table_resolution mapping error");
  }
// select filter:
  auto filter_type_str = parser.get_option("--filter_type");
  if ( !filter_type_str.empty()) {
    std::map<std::string, filter_t> table = {
      {"fast", filter_t::average_fast},
      {"average", filter_t::average},
      {"accurate", filter_t::average_accurate},
    };
    try {
      tv::filter_type = table.at(filter_type_str);
    } catch(...) {
      error("init: filter_type mapping error");
    }
  }
  switch (tv::filter_type)  {
    case filter_t::average_accurate: tv_signal_filter = filter_accurate; break;
    case filter_t::average_fast: tv_signal_filter = filter_fast; break;
    case filter_t::average:
    default: tv_signal_filter = filter; break;
  }
// select ringing type
  auto ringing_type_str = parser.get_option("--ringing_type");
  if ( !ringing_type_str.empty()) {
    std::map<std::string, ft_ringing> table = {
      {"right", ringing},
      {"bidir", ringing_bidirect},
    };
    try {
      tv_ringing = table.at(ringing_type_str);
    } catch(...) {
      error("init: tv_ringing mapping error");
    }
  } else
    tv_ringing = ringing; // default
// select tv class
  auto tv_type_str = parser.get_option("--tv_type");
  if ( !tv_type_str.empty()) {
    std::map<std::string, tv_t> table = {
      {"color", tv_t::yuv},
      {"bw", tv_t::bw},
    };
    try {
      tv::tv_type = table.at(tv_type_str);
    } catch(...) {
      error("init: tv_type mapping error");
    }
  }
  switch (tv::tv_type) {
    case tv_t::yuv: {
      str_info += "enabled TV-YUV mode\n";
      info.color_type = seze_RGB24; // TVsim_YUV сам сконвертит в YUV
      break;
    }
    case tv_t::rgb: {
      str_info += "enabled VGA-RGB mode\n";
      info.color_type = seze_RGB24;
      error("tv_t::rgb add impl");
      break;
    }
    case tv_t::bw:
    default: {
      str_info += "enabled TV-GRAY mode\n";
      info.color_type = seze_gray;
      break;
    }
  } // switch type
  info.info = str_info.c_str();
  return info;
#undef SET_F_IF
#undef SET_I_IF
} // init

static void prepare_tvsim(int mx, int my) {
  if (simulator)
    return;
  switch (tv::tv_type) {
    case tv_t::yuv: simulator = new TVsim_YUV(mx, my); break;
    case tv_t::rgb: error("tv_t::rgb add impl");
    case tv_t::bw: simulator = new TVsim_BW(mx, my); break;
  } // switch type
} // prepare_tvsim

void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  iferror( !dst, "core: dst is null");
  prepare_tvsim(mx, my);
  seze::Image dst_pic(dst, mx, my, color_type);
  simulator->process(dst_pic);
} // core

void finalize() { delete simulator; }

void TVsim::check_config() {
  // check settings:
  iferror(tv::first_bound < 0, "TVsim_BW: first_bound < 0");
  iferror(tv::second_bound < 0, "TVsim_BW: second_bound < 0");
  iferror(tv::vsync_detect < tv::hsync_size,
    "TVsim_BW: vsync detect < hsync");
  iferror(tv::hsync_size >= tv::vsync_size, "TVsim_BW: hsync >= vsync");
  iferror(tv::hsync_size <= 0, "TVsim_BW: hsync <= 0");
  iferror(tv::vsync_size <= 0, "TVsim_BW: vsync <= 0");
  iferror(tv::black_level >= tv::white_level,
    "TVsim_BW: black lvl >= white lvl");
  iferror(tv::blank_level >= tv::black_level,
    "TVsim_BW: blank lvl >= black lvl");
  iferror(tv::sync_level >= tv::blank_level,
    "TVsim_BW: sync lvl >= blank lvl");
  iferror(tv::AM_ratio_in == 0, "TVsim_BW: AM ratio input = 0");
  iferror(tv::AM_ratio_in_u == 0, "TVsim_BW: AM ratio input u = 0");
  iferror(tv::AM_ratio_in_v == 0, "TVsim_BW: AM ratio input v = 0");
  iferror(tv::AM_ratio_out == 0, "TVsim_BW: AM ratio output = 0");
  iferror(tv::AM_ratio_out_u == 0, "TVsim_BW: AM ratio output u = 0");
  iferror(tv::AM_ratio_out_v == 0, "TVsim_BW: AM ratio output v = 0");
  iferror(tv::beam_speed <= 0, "TVsim_BW: beam speed <= 0");
  iferror(tv::beam_reverse_speed <= 0, "TVsim_BW: beam reverse speed <= 0");
  iferror(tv::hbound < 0, "TVsim_BW: hbound < 0");
  iferror(tv::power < 0, "TVsim_BW: power < 0");
  iferror(tv::power_u < 0, "TVsim_BW: power v < 0");
  iferror(tv::power_v < 0, "TVsim_BW: power u < 0");
  iferror(tv::power_r < 0, "TVsim_BW: power r < 0");
  iferror(tv::power_g < 0, "TVsim_BW: power g < 0");
  iferror(tv::power_b < 0, "TVsim_BW: power b < 0");
  tv::empty_level = (tv::black_level
    + tv::blank_level) / component(2);
} // check_config
