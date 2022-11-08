#pragma once
/**
 * @brief SEZE II внутреннее API плагинов
 * @version 2.0
 * @date 2021-12-26
 */
#include "color-type.h"
#include <stdint.h>

// enable multithread processing
#define PLGNINF_MULTITHREAD 1 << 0

struct PluginInfo {
  const char* title; ///< plugin name
  const char* info; ///< instruction of using
  unsigned version; ///< plugin ver
  int in_x, in_y; ///< internal framebuffer resolution
  enum color_t color_type; ///< color type for plugin
  unsigned flags;
};
