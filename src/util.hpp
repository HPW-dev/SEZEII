#pragma once
#include "utils/macro.hpp"
#include "utils/types.hpp"

namespace seze {
  class Image;
  class Plugin;
}
struct SDL_Surface;
struct PluginInfo;

void parse_args(int argc, char** argv);
void image_to_surface(CN(seze::Image) src, SDL_Surface *dst);
void enable_plugin_settings(PluginInfo &src);
void print_converting_info();
void print_plugin_info(CN(PluginInfo) x);
seze::Plugin* new_plugin_by_ext(CN(Str) fname);
