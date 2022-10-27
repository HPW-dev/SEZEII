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
void correct_plugin_settings(PluginInfo &src);
void print_converting_info();
void print_plugin_info(CN(PluginInfo) x);
shared_p<seze::Plugin> new_plugin(CN(Str) fname);

#ifdef WINDOWS
#define SDL_MAIN int SDL_main(int argc, char* argv[])
#else
#define SDL_MAIN int main(int argc, char* argv[])
#endif
