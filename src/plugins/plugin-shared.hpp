#pragma once
#include "plugin.hpp"

class DyLib;

namespace seze {

class PluginShared: public Plugin {
  DyLib* lib = nullptr; ///< shared funcs loader
  ft_init f_init = {};
  ft_core f_core = {};
  ft_finalize f_finalize = {};

public:
  PluginShared(CN(std::string) fname);
  ~PluginShared();
  PluginInfo init(CN(std::string) options) override;
  void core(byte* dst, int mx, int my, int stride,
    color_t color_type) override;
  void finalize() override;

}; // PluginShared

} // seze ns
