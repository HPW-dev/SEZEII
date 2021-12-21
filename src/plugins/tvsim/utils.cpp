#include "utils.hpp"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <cmath>

void interpolate(Stream& dst, CN(Stream) src) {
  if (src.size == dst.size)
    return;
  FOR (x, dst.size-1) {
    float gx = x / component(dst.size) * src.size;
    int gxi = std::floor(gx);
    auto a = src.data[gxi];
    auto b = src.data[gxi + 1];
    dst.data[x] = std::lerp(a, b, component(gx - gxi));
  }
}

void modulate(Stream& dst, component freg) {
  FOR (i, dst.size) {
    component& x = dst.data[i];
    x *= std::sin(freg * i);
  }
}

void demodulate(Stream& dst, component freg,
component shift) {
  FOR (i, dst.size) {
    component& x = dst.data[i];
    auto wave = std::sin(freg * i + shift);
    x = wave == 0 ? 0 : x / wave;
  }
}

void apply_noise(Stream& dst, component noise_level, component power,
component shift) {
  for (auto ptr = dst.data.data();
  ptr != dst.data.data() + dst.size; ++ptr) {
    // TODO randoms for "component"
    #ifdef TVSIM_LD_COMPONENT
    auto val = seze::ldrand(-noise_level, noise_level);
    #endif
    #ifdef TVSIM_FLOAT_COMPONENT
    auto val = seze::frand_fast(-noise_level, noise_level);
    #endif
    *ptr = (*ptr + val + shift) * power;
  }
} // apply_noise

component RGB24_to_component(CN(seze::RGB24) src) {
  return (component(src.R + src.G + src.B) / component(3))
    / component(255);
}

seze::RGB24 component_to_RGB24(component src) {
  auto luma = std::clamp<int>(src * component(255), 0, 255);
  return seze::RGB24(luma, luma, luma);
}

component byte_to_component(byte src)
{ return component(src) / component(255); }

byte component_to_byte(component src) {
  auto luma = std::clamp<int>(src * component(255), 0, 255);
  return luma;
}

void gray_to_RGB24(seze::Image& dst, CN(seze::Image) src) {
  FOR (i, src.size()) {
    auto c = component_to_RGB24(src.fast_get<component>(i));
    dst.fast_set<seze::RGB24>(i, c);
  }
}

void RGB24_to_gray(seze::Image& dst, CN(seze::Image) src) {
  FOR (i, src.size()) {
    auto luma = RGB24_to_component(src.fast_get<seze::RGB24>(i));
    dst.fast_set<component>(i, luma);
  }
}


void gray_to_fgray(seze::Image& dst, CN(seze::Image) src) {
  FOR (i, src.size()) {
    auto luma = byte_to_component(src.fast_get<byte>(i));
    dst.fast_set<component>(i, luma);
  }
}

void fgray_to_gray(seze::Image& dst, CN(seze::Image) src) {
  FOR (i, src.size()) {
    auto luma = component_to_byte(src.fast_get<component>(i));
    dst.fast_set<byte>(i, luma);
  }
}

void filter(Stream& dst, int window) {
  if (window < 1)
    return;
  auto mul_doubled_wnd = component(1) / (window * 2);
  for (auto global_ptr = dst.data.data() + window;
  global_ptr != dst.data.data() + dst.size - window;
  ++global_ptr) {
    component total = 0;
    for (auto local_ptr = global_ptr - window;
    local_ptr != global_ptr + window; ++local_ptr)
      total += *local_ptr;
    *global_ptr = total * mul_doubled_wnd;
  }
}

void filter_fast(Stream& dst, int window) {
  if (window < 1)
    return;
  component fwindow = component(1) / window;
  FOR(i, dst.size - window) {
    component total = 0;
    FOR (j, window)
      total += dst.data[i + j];
    dst.data[i] = total * fwindow;
  }
}

void filter_accurate(Stream& dst, int window) {
  if (window < 1)
    return;
  auto src(dst.data);
  for (int i = window; i < dst.size - window; ++i) {
    component total = 0;
    for (int j = -window; j < window; ++j)
      total += src[i + j];
    dst.data[i] = total / (window * 2);
  }
}

component_yuv RGB24_to_yuv(CN(seze::RGB24) c) {
  constexpr auto mul = component(1) / component(255);
  auto fr = c.R * mul;
	auto fg = c.G * mul;
	auto fb = c.B * mul;
	auto Y = 0.2989 * fr + 0.5866 * fg + 0.1145 * fb;
	auto Cb = -0.1687 * fr - 0.3313 * fg + 0.5000 * fb;
	auto Cr = 0.5000 * fr - 0.4184 * fg - 0.0816 * fb;
  return component_yuv(Y, Cb, Cr);
}

 seze::RGB24 yuv_to_RGB24(CN(component_yuv) c) {
  constexpr auto mul = component(255);
  auto r = std::clamp<component>(c.Y + 0.0000 * c.U + 1.4022 * c.V, 0, 1);
  r *= mul;
	auto g = std::clamp<component>(c.Y - 0.3456 * c.U - 0.7145 * c.V, 0, 1);
  g *= mul;
	auto b = std::clamp<component>(c.Y + 1.7710 * c.U + 0.0000 * c.V, 0, 1);
  b *= mul;
	return seze::RGB24(r, g, b);
}

component ringing(CN(Stream) dst, int i, component ratio, int len,
component power) {
  auto ret = dst.data[i];
  if (ratio <= 0)
    return ret;
  if (len <= 0)
    return ret;
  if (power <= 0)
    return ret;
  FOR (j, len) {
    auto idx = i + j;
    if (idx >= dst.size)
      break;
    auto alpha = std::lerp(component(1), component(0), j / component(len));
    auto mul = std::cos(ratio * j) * alpha;
    ret += dst.data[idx] * mul * power;
  }
  return ret;;
}

component ringing_bidirect(CN(Stream) dst, int i, component ratio,
int len, component power) {
  auto ret = dst.data[i];
  if (ratio <= 0)
    return ret;
  if (len <= 0)
    return ret;
  if (power <= 0)
    return ret;
  for (int j = -len; j < len; ++j) {
    auto idx = i + j;
    if (idx >= dst.size or idx < 0)
      break;
    auto alpha = std::lerp(component(1), component(0), j / component(len));
    auto mul = std::cos(ratio * j) * alpha;
    ret += dst.data[idx] * mul * power;
  }
  return ret;
}
