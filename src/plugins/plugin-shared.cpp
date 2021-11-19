#include "plugin-shared.hpp"
#include "../utils/error.hpp"
#include "../utils/log.hpp"
#include <DyLib.hpp>

namespace seze {

PluginShared::PluginShared(CN(std::string) fname)
: lib(new DyLib(fname)) {
  LOG("PluginShared: loading \"" << fname <<"\"\n");
  f_init = lib->getFunction<parm_init>("init");
  f_core = lib->getFunction<parm_core>("core");
  f_finalize = lib->getFunction<parm_finalize>("finalize");
  iferror( !f_init, "PluginShared: _init addr is null");
  iferror( !f_core, "PluginShared: _core addr is null");
  iferror( !f_finalize, "PluginShared: _finalize addr is null");
}

PluginShared::~PluginShared() { delete lib; }

PluginInfo PluginShared::init(CN(std::string) options)
{ return f_init(options); }

void PluginShared::core(byte* dst, int mx, int my, int stride,
color_t color_type)
{ f_core(dst, mx, my, stride, color_type); }

void PluginShared::finalize() { f_finalize(); }

} // seze ns
