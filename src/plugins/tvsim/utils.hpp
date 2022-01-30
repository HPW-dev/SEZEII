#pragma once
#include "utils/macro.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "utils/random.hpp"
#include "component.hpp"

void apply_noise(Stream& dst, component noise_level, component power=1,
  component shift=0);
void modulate(Stream& dst, component freg=1);
void demodulate(Stream& dst, component freg=1,
  component shift=0);
void interpolate(Stream& dst, CN(Stream) src);
void filter(Stream& dst, int window=1); ///< average filter
void filter_fast(Stream& dst, int window=1); ///< fast average filter
void filter_accurate(Stream& dst, int window=1); ///< median filter
component ringing(CN(Stream) dst, int i, component ratio=1, int len=16,
  component power = 0.075);
component ringing_bidirect(CN(Stream) dst, int i, component ratio=1.1, int len=16,
  component power = 0.09);  

component RGB24_to_component(CN(seze::RGB24) src);
seze::RGB24 component_to_RGB24(component src);
void gray_to_RGB24(seze::Image& dst, CN(seze::Image) src);
void RGB24_to_gray(seze::Image& dst, CN(seze::Image) src);
void gray_to_fgray(seze::Image& dst, CN(seze::Image) src);
void fgray_to_gray(seze::Image& dst, CN(seze::Image) src);
component_yuv RGB24_to_yuv(CN(seze::RGB24) c);
seze::RGB24 yuv_to_RGB24(CN(component_yuv) c);
