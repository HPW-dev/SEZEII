# How to build plugin
Download [SEZE II plugin API](../plugin-api.hpp) and create your plugin on C++.\
See [my plugins](../src/plugins) or use this example of color invesion:
```cpp
// invert.cpp
#include "plugin-api.hpp"
#include <algorithm>

// for C-style func names in shared lib
extern "C" {
  PluginInfo init(const std::string& options);
  void core(byte* dst, int x, int y, int stride, color_t color_type);
  void finalize();
}

//! plugin initialization
PluginInfo init(const std::string& options) {
  PluginInfo info;
  info.color_type = color_t::RGB24;
  info.title = "Color inversion";
  return info;
}

//! frame processing
void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  auto size_in_bytes = my * stride;
  // invert all pixel components
  std::transform(dst, dst + size_in_bytes, dst,
    [](byte pix)->byte { return ~pix; });
}

//! plugin close
void finalize() {}
```
For build plugin for Windows: ```g++ -shared invert.cpp -o inversion.dll```\
For build plugin for Linux: ```g++ -shared invert.cpp -o inversion.so```
