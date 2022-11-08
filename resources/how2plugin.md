# How to build plugin
Download [SEZE II plugin API](../plugin-api.h) and create your plugin on C++.\
See [my plugins](../src/plugins) or use this example of color invesion:
```c
// invert.c
#include "plugin-api.h"

// plugin initialization
struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "color inversion";
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
void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  int bytes = my * stride;
  for (int i = 0; i < bytes; ++i)
    dst[i] = ~dst[i];
}

// plugin close
void finalize() {}
```
For build plugin: ```g++ -shared invert.c -o invert.dll```\
Launch ```seze``` or ```seze-gui``` and find your plugin & test it
