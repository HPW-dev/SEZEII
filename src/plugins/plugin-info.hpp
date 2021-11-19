#pragma once
/**
 * @brief SEZE II внутреннее API плагинов
 * @version 1.0
 * @date 2021-11-18
 */
#include "color-type.hpp"
#include "../utils/macro.hpp"
#include <string>

namespace seze {

struct PluginInfo {
  std::string title = {};
  std::string info = {}; ///< instruction of using
  uint version = 1;
  int in_x = 0, in_y = 0; ///< internal framebuffer resolution
  color_t color_type = {}; ///< needed color type
  //! if tru, use multithread processing in converter
  bool multithread = true;
};

} // seze ns
