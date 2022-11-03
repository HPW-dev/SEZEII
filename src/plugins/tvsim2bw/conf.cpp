#include <sstream>
#include "conf.hpp"

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
  if (conf.fix_opts) ss << " --fix_opts";
  if (conf.interlacing) ss << " --interlacing";
  if (conf.use_fading) ss << " --use_fading";
  if (conf.use_scale) ss << " --use_scale";
  if (conf.use_am) ss << " --use_am";
  if (conf.debug) ss << " --debug";
  if (conf.debug_black_bg) ss << " --debug_black_bg";
  return ss.str();
} // conf_to_opts

void opts_to_conf(CN(Str) str, tvsim_conf &conf) {

}
