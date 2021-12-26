#pragma once
#include "plugins/plugin-shared.hpp"
#include "utils/log.hpp"
#include "utils/error.hpp"
#include "utils/str.hpp"
#include <map>
#include <filesystem>

extern Str iname; // input video file
extern Str oname; // output video file
extern Str pname; // plugin file
extern Str popts; // plugin options
extern uint num_threads;
extern int width, height; // if not 0, custom output video size
extern int framebuffer_x, framebuffer_y;
extern seze::plugin_format_t plugin_format;
extern bool nout; // if 1, disable video output
extern bool nrend; // if 1, disable rendering output framebuffer
extern bool simple_encoder; // if 1, enable h264 crf 23 nv12

//! guess plugin format by file extension
seze::Plugin* new_plugin_by_ext(CN(Str) fname);
void print_help();
void opts_check();
bool parse_args(int argc, char* argv[]);
void print_converting_info();
void print_plugin_info(CN(PluginInfo) x);
void enable_plugin_settings(CN(PluginInfo) src);
