# How to build plugin
Download [SEZE II plugin API](../plugin-api.h) and create your plugin on C++.\
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

// plugin initialization
PluginInfo init(const std::string& options) {
  PluginInfo info;
  info.color_type = seze_RGB24;
  info.title = "Color inversion";
  return info;
}

/*
frame processing
dst - RAW pixel data for input frame 
mx - width of input frame
my - height of input frame
stride - size of pixel string (mx * pixel data size)
color_type - type of pixel data
*/
void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  auto size_in_bytes = my * stride;
  // invert all pixel components
  std::transform(dst, dst + size_in_bytes, dst,
    [](byte pix)->byte { return ~pix; });
}

// plugin close
void finalize() {}
```
For build plugin: ```g++ -shared invert.cpp -o invert.dll```\
Launch ```seze``` or ```seze-gui``` and find your plugin & test it
