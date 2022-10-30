#pragma once
/** @brief SEZE II plugin API
  * @author HPW-dev (hpwdev0@gmail.com)
  * @version 2.0
  * @date 2022-10-30
  * @copyright Copyright (c) 2021-2022, HPW-dev */
#include <stdint.h>

typedef unsigned uint;
typedef uint8_t byte;

#ifndef COLOR_T
#define COLOR_T
//! avaliable color formats
enum color_t {
  seze_none,
  seze_RAW,
  seze_RGB8,
  seze_RGB24, ///< RGB 8:8:8
  seze_BGR24, ///< BGR 8:8:8
  seze_RGB555, ///< RGB 5:5:5
  seze_RGB565, ///< RGB 5:6:5
  seze_YUV24, ///< YUV 4:4:4 (24bit)
  seze_YUVf,
  seze_YUVld,
  seze_RGBi,
  seze_RGBf,
  seze_RGBld,
  seze_f_gray,
  seze_ld_gray,
  seze_gray, ///< 1 byte grayscale
}; // color_t
#endif

// enable multithread processing
#define PLGNINF_MULTITHREAD 1 << 0

struct PluginInfo {
  const char* title; ///< plugin name
  const char* info; ///< instruction of using
  uint version; ///< plugin ver
  int in_x, in_y; ///< internal framebuffer resolution
  enum color_t color_type; ///< color type for plugin
  uint flags;
};

inline static void PluginInfo_init(struct PluginInfo* info) {
  info->title = 0;
  info->info = 0;
  info->version = 2;
  info->in_x = 0;
  info->in_y = 0;
  info->color_type = seze_none;
  info->flags = PLGNINF_MULTITHREAD;
}

inline static void bit_enable(uint* flag, uint bit)
{ *flag |= bit; }

inline static void bit_disable(uint* flag, uint bit)
{ *flag &= ~bit; }

inline static void bit_set_if(uint* flag, uint bit, int cond) {
  if (cond)
    bit_enable(flag, bit);
  else
    bit_disable(flag, bit);
}

struct PluginInfo init(const char* options);
void core(byte* dst, int mx, int my, int stride, enum color_t color_type);
void finalize();
