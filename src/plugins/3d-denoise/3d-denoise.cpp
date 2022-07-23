#include <omp.h>
#include <cstring>
#include <vector>
#include <numeric>
#include <algorithm>
extern "C" {
#include "plugin-api.h"
}
#include "utils/log.hpp"
#include "utils/macro.hpp"
#include "utils/aalloc.hpp"
#include "utils/error.hpp"

enum class Mode { avr, avr_fast, med };

// configs:
static Mode mode_ = Mode::med;
static uint samples_ = 60;

static uint cur_sample = 0;
static byte** buffer_3d={};
static uint buffer_3d_size = 0;
static byte* avr_buf={};

static void next_sample()
  { cur_sample = (cur_sample + 1) % samples_; }

// усреднение
static void sharpen_average_fast(byte* dst, int bytes) {
  FOR (i, bytes) {
    auto col = (dst[i] + avr_buf[i]) >> 1;
    dst[i] = col;
    avr_buf[i] = col;
  }
} // sharpen_average

static void sharpen_average(byte* dst, int bytes) {
  FOR (i, bytes)
    buffer_3d[i][cur_sample] = dst[i];
  #pragma omp parallel for simd schedule(static, 8)
  FOR (byte_i, bytes) 
    dst[byte_i] = std::accumulate(buffer_3d[byte_i],
      buffer_3d[byte_i] + samples_, 0) / samples_;
  next_sample();
} // sharpen_average

static void sharpen_median(byte* dst, int bytes) {
  FOR (i, bytes)
    buffer_3d[i][cur_sample] = dst[i];
  #pragma omp parallel for simd schedule(static, 8)
  FOR (byte_i, bytes) {
    auto median = samples_/2;
    std::nth_element(buffer_3d[byte_i],
      buffer_3d[byte_i] + median, buffer_3d[byte_i] + samples_);
    dst[byte_i] = buffer_3d[byte_i][median];
  }
  next_sample();
} // sharpen_median

struct PluginInfo init(CP(char) options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "3D Denoiser";
  bit_disable(&info.flags, PLGNINF_MULTITHREAD);
  iferror(samples_ < 1, "3d-denoise.init: samples_ < 1");
  return info;
}

void core(byte* dst, int /*mx*/, int my, int stride,
color_t /*color_type*/) {
  int bytes = my * stride;
  // выделить буфферы в первый раз. Если avr, то 1 буффер
  if ( !avr_buf)
    avr_buf = rcast(byte*, aalloc(64, bytes));
  if ( !buffer_3d) {
    buffer_3d_size = bytes;
    buffer_3d = rcast(decltype(buffer_3d),
      aalloc(64, bytes * sizeof(byte*)));
    FOR (i, bytes) {
      buffer_3d[i] = rcast(byte*, aalloc(64, samples_));
      FOR (j, samples_)
        buffer_3d[i][j] = dst[i];
    }
  }
  switch (mode_) {
    case Mode::med: sharpen_median(dst, bytes); break;
    case Mode::avr: sharpen_average(dst, bytes); break;
    case Mode::avr_fast:
    default: sharpen_average_fast(dst, bytes); break;
  }
} // core

void finalize() {
  FOR (i, buffer_3d_size)
    afree(buffer_3d[i]);
  afree(buffer_3d);
  afree(avr_buf);
}
