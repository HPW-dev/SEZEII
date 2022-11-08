#include <sstream>
#include "conf.hpp"
#include "utils/pparser.hpp"

Str conf_to_opts(CN(tvsim_conf) conf) {
  std::stringstream ss;
  ss << " -w " << conf.scale_wh.x;
  ss << " -h " << conf.scale_wh.y;
  ss << " --desat_type " << int(conf.desat_type);
  ss << " --scale_type_in " << int(conf.scale_type_in);
  ss << " --scale_type_out " << int(conf.scale_type_out);
  ss << " --hfront " << conf.hfront;
  ss << " --hback " << conf.hback;
  ss << " --hsync_sz " << conf.hsync_sz;
  ss << " --vfront " << conf.vfront;
  ss << " --vback " << conf.vback;
  ss << " --vsync_sz " << conf.vsync_sz;
  ss << " --vsync_needed_cnt " << conf.vsync_needed_cnt;
  ss << " --beam_spd_x " << conf.beam_spd_x;
  ss << " --beam_spd_y " << conf.beam_spd_y;
  ss << " --beam_spd_back " << conf.beam_spd_back;
  ss << " --white_lvl " << conf.white_lvl;
  ss << " --black_lvl " << conf.black_lvl;
  ss << " --beam_off_signal " << conf.beam_off_signal;
  ss << " --sync_lvl " << conf.sync_lvl;
  ss << " --sync_signal " << conf.sync_signal;
  ss << " --fading " << conf.fading;
  ss << " --pre_amp " << conf.pre_amp;
  ss << " --amp " << conf.amp;
  ss << " --filter_type " << int(conf.filter_type);
  ss << " --filter_power " << conf.filter_power;
  ss << " --noise_level " << conf.noise_level;
  ss << " --am_freg " << conf.am_freg;
  ss << " --am_depth " << conf.am_depth;
  ss << " --am_tune " << conf.am_tune;
  ss << " --fix_opts" << int(conf.fix_opts);
  ss << " --interlacing" << int(conf.interlacing);
  ss << " --use_fading" << int(conf.use_fading);
  ss << " --use_scale" << int(conf.use_scale);
  ss << " --use_am" << int(conf.use_am);
  ss << " --debug" << int(conf.debug);
  ss << " --debug_black_bg" << int(conf.debug_black_bg);
  return ss.str();
} // conf_to_opts

void opts_to_conf(CN(Str) str, tvsim_conf &conf) {
  seze::pparser parser({
    {{"-w"}, "simulated frame width", [&conf](CN(Str) opt) { conf.scale_wh.x = std::stoi(opt); } },
    {{"-h"}, "simulated frame height", [&conf](CN(Str) opt) { conf.scale_wh.y = std::stoi(opt); } },
    {{"--desat_type"}, "desaturation type", [&conf](CN(Str) opt) { conf.desat_type = desaturation_e(std::stoi(opt)); } },
    {{"--scale_type_in"}, "scale filter for input", [&conf](CN(Str) opt) { conf.scale_type_in = scale_e(std::stoi(opt)); } },
    {{"--scale_type_out"}, "scale filter for output", [&conf](CN(Str) opt) { conf.scale_type_out = scale_e(std::stoi(opt)); } },
    {{"--hfront"}, "horizontal front porch lenght", [&conf](CN(Str) opt) { conf.hfront = std::stoi(opt); } },
    {{"--hback"}, "horizontal back porch lenght", [&conf](CN(Str) opt) { conf.hback = std::stoi(opt); } },
    {{"--hsync_sz"}, "horizontal sync lengh", [&conf](CN(Str) opt) { conf.hsync_sz = std::stoi(opt); } },
    {{"--vfront"}, "vertical front porch lenght", [&conf](CN(Str) opt) { conf.vfront = std::stoi(opt); } },
    {{"--vback"}, "vertical back porch lenght", [&conf](CN(Str) opt) { conf.vback = std::stoi(opt); } },
    {{"--vsync_sz"}, "vertical sync lengh", [&conf](CN(Str) opt) { conf.vsync_sz = std::stoi(opt); } },
    {{"--vsync_needed_cnt"}, "Vsync detection count", [&conf](CN(Str) opt) { conf.vsync_needed_cnt = std::stoi(opt); } },
    {{"--beam_spd_x"}, "beam speed by x", [&conf](CN(Str) opt) { conf.beam_spd_x = std::stof(opt); } },
    {{"--beam_spd_y"}, "beam speed by y", [&conf](CN(Str) opt) { conf.beam_spd_y = std::stof(opt); } },
    {{"--beam_spd_back"}, "beam reverse speed", [&conf](CN(Str) opt) { conf.beam_spd_back = std::stof(opt); } },
    {{"--white_lvl"}, "white signal level", [&conf](CN(Str) opt) { conf.white_lvl = std::stof(opt); } },
    {{"--black_lvl"}, "black signal level", [&conf](CN(Str) opt) { conf.black_lvl = std::stof(opt); } },
    {{"--beam_off_signal"}, "signal value for empty pixel (beam off)", [&conf](CN(Str) opt) { conf.beam_off_signal = std::stof(opt); } },
    {{"--sync_lvl"}, "sync signal level", [&conf](CN(Str) opt) { conf.sync_lvl = std::stof(opt); } },
    {{"--sync_signal"}, "sync signal value", [&conf](CN(Str) opt) { conf.sync_signal = std::stof(opt); } },
    {{"--fading"}, "speed of beam fade", [&conf](CN(Str) opt) { conf.fading = std::stof(opt); } },
    {{"--pre_amp"}, "signal pre-amplify", [&conf](CN(Str) opt) { conf.pre_amp = std::stof(opt); } },
    {{"--amp"}, "signal amplify", [&conf](CN(Str) opt) { conf.amp = std::stof(opt); } },
    {{"--filter_type"}, "filtration algo.", [&conf](CN(Str) opt) { conf.filter_type = filter_e(std::stoi(opt)); } },
    {{"--filter_power"}, "filtration power", [&conf](CN(Str) opt) { conf.filter_power = std::stoi(opt); } },
    {{"--noise_level"}, "noise level", [&conf](CN(Str) opt) { conf.noise_level = std::stof(opt); } },
    {{"--am_freg"}, "AM frequency", [&conf](CN(Str) opt) { conf.am_freg = std::stof(opt); } },
    {{"--am_depth"}, "AM depth", [&conf](CN(Str) opt) { conf.am_depth = std::stof(opt); } },
    {{"--am_tune"}, "AM tune value", [&conf](CN(Str) opt) { conf.am_tune = std::stof(opt); } },
    {{"--fix_opts"}, "autofix options", [&conf](CN(Str) opt) { conf.fix_opts = bool(stoi(opt)); } },
    {{"--interlacing"}, "use interlacin", [&conf](CN(Str) opt) { conf.interlacing = bool(stoi(opt)); } },
    {{"--use_fading"}, "use beam fading", [&conf](CN(Str) opt) { conf.use_fading = bool(stoi(opt)); } },
    {{"--use_scale"}, "use scaling", [&conf](CN(Str) opt) { conf.use_scale = bool(stoi(opt)); } },
    {{"--use_am"}, "use AM-modulation", [&conf](CN(Str) opt) { conf.use_am = bool(stoi(opt)); } },
    {{"--debug"}, "use debug osc.", [&conf](CN(Str) opt) { conf.debug = bool(stoi(opt)); } },
    {{"--debug_black_bg"}, "use black background", [&conf](CN(Str) opt) { conf.debug_black_bg = bool(stoi(opt)); } },
  });
  parser(str);
} // opts_to_conf

Str conf_to_opts(CN(tvsim_conf_yuv) conf_yuv) {
  return {}; // TODO
}

void opts_to_conf(CN(Str) str, tvsim_conf_yuv &conf_yuv) {
  // TODO
}
