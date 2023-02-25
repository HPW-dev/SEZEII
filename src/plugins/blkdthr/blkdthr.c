#include <stddef.h>
#include <math.h>
#include "plugin-api.h"

static size_t state;
static byte max_luma;
typedef float real;

struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_gray;
  info.title = "Blink dithering (BW)";
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  max_luma = 0;
  state = 0;
  return info;
}

void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  int bytes = my * stride;
  
  #define max_table_str 4
  #define max_table 5
  static const byte table [max_table][max_table_str] = {
    {0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0xFF},
    {0x00,0xFF,0x00,0xFF},
    {0x00,0xFF,0xFF,0xFF},
    {0xFF,0xFF,0xFF,0xFF}
  };
  uint idx = state % max_table_str;

  for (int i = 0; i < bytes; ++i) {
    real alpha = max_luma / 5.0;
    byte luma = dst[i];
    // blink dither
    size_t str_idx = 0;
    if (luma > (byte)(alpha * 1.0) )
      str_idx = 1;
    if (luma >= (byte)(alpha * 2.0) )
      str_idx = 2;
    if (luma >= (byte)(alpha * 3.0) )
      str_idx = 3;
    if (luma >= (byte)(alpha * 4.0) )
      str_idx = 4;
    if (max_luma < luma)
      max_luma = luma;

    luma = table[str_idx][idx];
    dst[i] = luma;
  }

  ++state;
} // core

void finalize() {}
