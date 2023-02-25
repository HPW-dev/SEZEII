#pragma once
#include <cmath>
#include "global.hpp"
#include "Image/Image.hpp"
#include "image/rgb24.hpp"
#include "math.hpp"

bool check_color(CN(seze::RGB24) a, CN(seze::RGB24) b);
void find_point(CN(seze::Image) src, Point& dst, CN(seze::RGB24) color);
void find_points(CN(seze::Image) src);
Point calc_center(CN(seze::Image) dst, CN(seze::Image) src);
void stretch(seze::Image& dst);

inline seze::RGB24 neighbor_core(CN(seze::Image) src, Real gx, Real gy)
  { return src.get<seze::RGB24>(int(gx), int(gy)); }

inline seze::RGB24 neighbor_1d_core(CN(v_pixel_t) src, Real gi) {
  auto i = std::clamp<int>(gi, 0, src.size());
  return src[i];
}

inline seze::RGB24 lerp(CN(seze::RGB24) a, CN(seze::RGB24) b, Real t) {
  return seze::RGB24 {
    byte(std::lerp(a.R, b.R, t)),
    byte(std::lerp(a.G, b.G, t)),
    byte(std::lerp(a.B, b.B, t)),
  };
}

inline seze::RGB24 blerp(CN(seze::RGB24) c00, CN(seze::RGB24) c10,
CN(seze::RGB24) c01, CN(seze::RGB24) c11, Real tx, Real ty)
  { return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty); }

inline Real bcerp(Real A, Real B, Real C, Real D, Real t)
  { return B + 0.5 * t * (C - A + t * (2.0 * A - 5.0 * B + 4.0 * C - D + t * (3.0 * (B - C) + D - A))); }

inline seze::RGB24 bcerp(CN(seze::RGB24) A, CN(seze::RGB24) B,
CN(seze::RGB24) C, CN(seze::RGB24) D, Real t) {
  return {
    byte(std::clamp<int>(bcerp(A.R, B.R, C.R, D.R, t), 0, 255)),
    byte(std::clamp<int>(bcerp(A.G, B.G, C.G, D.G, t), 0, 255)),
    byte(std::clamp<int>(bcerp(A.B, B.B, C.B, D.B, t), 0, 255))
  };
}

inline seze::RGB24 linear_core(CN(v_pixel_t) src, Real gi) {
  auto i0 = std::clamp<int>(gi, 0, src.size());
  auto i1 = std::clamp<int>(gi+1, 0, src.size());
  auto fx = gi - i0;
  return lerp(src[i0], src[i1], fx);
}

inline seze::RGB24 cubic_core(CN(v_pixel_t) src, Real gi) {
  auto i0 = std::clamp<int>(gi-1, 0, src.size());
  auto i1 = std::clamp<int>(gi+0, 0, src.size());
  auto i2 = std::clamp<int>(gi+1, 0, src.size());
  auto i3 = std::clamp<int>(gi+2, 0, src.size());
  auto fx = gi - i1;
  return bcerp(src[i0], src[i1], src[i2], src[i3], fx);
}

inline seze::RGB24 bilinear_core(CN(seze::Image) src, Real gx, Real gy) {
  int ix = gx;
	int iy = gy;
	Real fx = gx - ix;
	Real fy = gy - iy;
  auto c00 = src.get<seze::RGB24>(ix+0, iy+0);
  auto c01 = src.get<seze::RGB24>(ix+0, iy+1);
  auto c10 = src.get<seze::RGB24>(ix+1, iy+0);
  auto c11 = src.get<seze::RGB24>(ix+1, iy+1);
  return blerp(c00, c10, c01, c11, fx, fy);
}

inline seze::RGB24 bicubic_core(CN(seze::Image) src, Real gx, Real gy) {
  int ix = gx;
	int iy = gy;
	Real fx = gx - ix;
	Real fy = gy - iy;
  auto c00 = src.get<seze::RGB24>(ix+0, iy+0);
  auto c01 = src.get<seze::RGB24>(ix+0, iy+1);
  auto c10 = src.get<seze::RGB24>(ix+1, iy+0);
  auto c11 = src.get<seze::RGB24>(ix+1, iy+1);
// 16 points:
  // 1st row
  auto p00 = src.get<seze::RGB24>(ix - 1, iy - 1);   
  auto p10 = src.get<seze::RGB24>(ix + 0, iy - 1);   
  auto p20 = src.get<seze::RGB24>(ix + 1, iy - 1);   
  auto p30 = src.get<seze::RGB24>(ix + 2, iy - 1);   
  // 2nd row
  auto p01 = src.get<seze::RGB24>(ix - 1, iy + 0);   
  auto p11 = src.get<seze::RGB24>(ix + 0, iy + 0);   
  auto p21 = src.get<seze::RGB24>(ix + 1, iy + 0);   
  auto p31 = src.get<seze::RGB24>(ix + 2, iy + 0);
  // 3rd row
  auto p02 = src.get<seze::RGB24>(ix - 1, iy + 1);   
  auto p12 = src.get<seze::RGB24>(ix + 0, iy + 1);   
  auto p22 = src.get<seze::RGB24>(ix + 1, iy + 1);   
  auto p32 = src.get<seze::RGB24>(ix + 2, iy + 1);
  // 4th row
  auto p03 = src.get<seze::RGB24>(ix - 1, iy + 2);   
  auto p13 = src.get<seze::RGB24>(ix + 0, iy + 2);   
  auto p23 = src.get<seze::RGB24>(ix + 1, iy + 2);   
  auto p33 = src.get<seze::RGB24>(ix + 2, iy + 2);
// interpolate bi-cubically:
  auto c0 = bcerp(p00, p10, p20, p30, fx);
  auto c1 = bcerp(p01, p11, p21, p31, fx);
  auto c2 = bcerp(p02, p12, p22, p32, fx);
  auto c3 = bcerp(p03, p13, p23, p33, fx);
  return bcerp(c0, c1, c2, c3, fy);
} // bicubic_core

template <auto core>
void rotate_templ(CN(seze::Image) src, seze::Image& dst,
CN(Point) pos, Real radian) {
  Real mul_x = std::cos(radian);
  Real mul_y = std::sin(radian);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    auto rot_x = mul_x * (x - pos.x) - mul_y * (y - pos.y) + pos.x;
    auto rot_y = mul_y * (x - pos.x) + mul_x * (y - pos.y) + pos.y;
    auto pix = core(src, rot_x, rot_y);
    dst.fast_set<seze::RGB24>(x, y, pix);
  }
} // rotate_templ

template <auto core>
void resize_templ(CN(seze::Image) src, seze::Image& dst, 
CN(Point) offset, int new_x, int new_y) {
  auto scale_x = Real(src.X) / new_x;
	auto scale_y = Real(src.Y) / new_y;
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    auto src_x = x * scale_x - offset.x;
    auto src_y = y * scale_y - offset.y;
    dst.fast_set<seze::RGB24>(x, y, core(src, src_x, src_y));
  }
} // resize_templ

//! растягивает один 1D массив цветов в другой
template <auto core>
void stretch_templ(CN(v_pixel_t) src, v_pixel_t& dst, int offset,
int new_size) {
  auto scale = Real(src.size()) / new_size;
  cfor (i, dst.size()) {
    auto src_i = i * scale - offset;
    dst[i] = core(src, src_i);
  }
}

#define rotate_fast rotate_templ<neighbor_core>
#define rotate_bilinear rotate_templ<bilinear_core>
#define rotate_bicubic rotate_templ<bicubic_core>
#define resize_fast resize_templ<neighbor_core>
#define resize_bilinear resize_templ<bilinear_core>
#define resize_bicubic resize_templ<bicubic_core>
#define stretch_fast stretch_templ<neighbor_1d_core>
#define stretch_linear stretch_templ<linear_core>
#define stretch_cubic stretch_templ<cubic_core>
