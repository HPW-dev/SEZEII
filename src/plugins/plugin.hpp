#pragma once
#include "plugin-info.hpp"
#include <functional>

namespace seze {

enum class plugin_format_t {
  unknown = 0,
  RPI, ///< Stive Snake format
  shared, ///< HPW-dev format
};

///@class base class 4 plugins
class Plugin {
public:
  //! use it for user c-tors. return - needed data types for plugin
  virtual PluginInfo init(CN(std::string) options) = 0;

  /** @param dst pixel data for processing
  * @param mx image width
  * @param my image height
  * @param stride stride = x * sizeof(pixel_data_type)
  * @param color_type image color type info */
  virtual void core(byte* dst, int mx, int my, int stride,
    color_t color_type) = 0;

  //! for user d-tor's & free data
  virtual void finalize() = 0;

  virtual ~Plugin();
}; // Plugin

using parm_init = PluginInfo(CN(std::string));
using parm_core = void(byte*, int, int, int, color_t);
using parm_finalize = void();
using ft_init = std::function<parm_init>;
using ft_core = std::function<parm_core>;
using ft_finalize = std::function<parm_finalize>;

} // seze ns
