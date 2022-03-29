#pragma once
#include "utils/macro.hpp"
#include "component.hpp"
#include <map>
#include <functional>

struct resolution_pair { int x = 0, y = 0; };
enum class resolution_e {CD_NTSC, CD_PAL, DVD_NTSC, UMD, SVCD, NTSC, PAL, RAW};
enum class filter_t {average, average_fast, average_accurate};
enum class tv_t {
  bw, ///< grayscale tv
  rgb, ///< RGB VGA
  yuv ///< colored tv
};
using ft_filter = std::function<void(Stream&, int)>;
using ft_ringing = std::function<component(CN(Stream), int,
  component, int, component)>;
extern ft_filter tv_signal_filter;
extern ft_ringing tv_ringing;

namespace {

std::map<resolution_e, resolution_pair> table_resolution = {
  {resolution_e::CD_NTSC, {352, 240}},
  {resolution_e::CD_PAL, {352, 288}},
  {resolution_e::NTSC, {486, 480}},
  {resolution_e::DVD_NTSC, {720, 540}},
  {resolution_e::PAL, {720, 576}},
  {resolution_e::SVCD, {480, 480}},
  {resolution_e::UMD, {480, 272}},
  {resolution_e::RAW, {-1, -1}}, ///< source RGB
};

} // ns ::

//! настройки для всех типов ТВ
namespace tv {
extern resolution_e resolution_type;
extern int first_bound; ///< size of first porch
extern int second_bound; ///< size of back porch
extern int hsync_size;
extern int vsync_size;
extern int vsync_detect; ///< число sync импульсов, после которого считается V-sync
extern int first_strings; ///< size of first v-bound
extern int second_strings; ///< size of last v-bound
extern component white_level;
extern component black_level;
extern component blank_level;
extern component sync_level;
extern component empty_level;
extern component noise_level; ///< noise level for luma
extern component noise_level_u;
extern component noise_level_v;
extern component AM_shift;
extern component AM_shift_u;
extern component AM_shift_v;
extern component AM_ratio_in; ///< sin(i * AM_ratio_in)
extern component AM_ratio_in_u;
extern component AM_ratio_in_v;
extern component AM_ratio_out; ///< sin(i * AM_ratio_out)
extern component AM_ratio_out_u;
extern component AM_ratio_out_v;
extern component beam_speed;
extern component beam_reverse_speed;
extern component hbound;
extern component fading;
extern component power; ///< piver of luma
extern component power_u;
extern component power_v;
extern component power_r;
extern component power_g;
extern component power_b;
extern component signal_shift;
extern component signal_shift_u;
extern component signal_shift_v;
extern bool interlace;
extern bool enable_AM;
extern filter_t filter_type;
extern int filter_power;
extern int filter_power_u;
extern int filter_power_v;
extern tv_t tv_type;
extern component ringing_ratio;
extern int ringing_length;
extern component ringing_power;
}