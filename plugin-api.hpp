#pragma once
/**
 * @brief SEZE II plugin API
 * @author HPW-dev (hpwdev0@gmail.com)
 * @version 1.0
 * @date 2021-11-18
 * @copyright Copyright (c) 2021, HPW-dev
 */
#include <cstdint>
#include <string>

using uint = unsigned;
using byte = uint8_t;

//! avaliable color formats
enum class color_t {
  _NOUSABLE_none=0,
  _NOUSABLE_RAW,
  _NOUSABLE_RGB8,
  RGB24, ///< RGB 8:8:8
  BGR24, ///< BGR 8:8:8
  RGB555, ///< RGB 5:5:5
  RGB565, ///< RGB 5:6:5
  YUV24, ///< YUV 4:4:4 (24bit)
  _NOUSABLE_YUVf,
  _NOUSABLE_YUVld,
  _NOUSABLE_RGBi,
  _NOUSABLE_RGBf,
  _NOUSABLE_RGBld,
  _NOUSABLE_f_gray,
  _NOUSABLE_ld_gray,
  gray, ///< 1 byte grayscale
}; // color_t

struct PluginInfo {
  std::string title = {}; ///< plugin name
  std::string info = {}; ///< instruction of using
  uint version = 1; ///< plugin ver
  int in_x = 0, in_y = 0; ///< internal framebuffer resolution
  color_t color_type = {}; ///< color type for plugin
  bool multithread = true; ///< enable multithread processing
};
