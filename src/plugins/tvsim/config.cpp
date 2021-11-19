#include "config.hpp"

ft_filter tv_signal_filter = nullptr;
ft_ringing tv_ringing = nullptr;

namespace tv {

resolution_e resolution_type = resolution_e::CD_NTSC;
int first_bound = 35;
int second_bound = 21;
int hsync_size = 65;
int vsync_size = 6571;
int vsync_detect = 100;
int first_strings = 120;
int second_strings = 80;
component white_level = component(1);
component black_level = component(0.1);
component blank_level = component(0);
component sync_level = component(-0.4);
component empty_level = (black_level + blank_level) / component(2);
component noise_level = component(0.02);
component noise_level_u = component(0.02);
component noise_level_v = component(0.02);
component AM_shift = component(0);
component AM_shift_u = component(0);
component AM_shift_v = component(0);
component AM_ratio_in = component(0.01);
component AM_ratio_in_u = component(0.01);
component AM_ratio_in_v = component(0.01);
component AM_ratio_out = component(0.01);
component AM_ratio_out_u = component(0.01);
component AM_ratio_out_v = component(0.01);
component beam_speed = component(1);
component beam_reverse_speed = component(10);
component hbound = component(4);
component fading = component(0.2);
component power = component(1.3);
component power_u = component(1);
component power_v = component(1);
component power_r = component(1);
component power_g = component(1);
component power_b = component(1);
component signal_shift = component(0.01);
component signal_shift_u = component(0);
component signal_shift_v = component(0);
bool interlace = true;
bool enable_AM = false;
filter_t filter_type = filter_t::average;
int filter_power = 2;
int filter_power_u = 5;
int filter_power_v = 5;
tv_t tv_type = tv_t::bw;
component ringing_ratio = component(1);
int ringing_length = 16;
component ringing_power = component(0);

} // config
