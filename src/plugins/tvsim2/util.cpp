#include <cmath>
#include <omp.h>
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"
#include "image/YUV.hpp"
#include "utils/random.hpp"

void desaturate(CN(seze::Image) src, seze::Image &dst,
desaturation_e type) {
  auto mul {1.0f / 255.0f};
  switch (type) {
    default:
    case desaturation_e::average: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        auto frgb {(col.R + col.G + col.B) * (1.0f / 3.0f)};
        dst.fast_get<luma_t>(i) = frgb * mul;
      }
      break;
    }
    case desaturation_e::bt2001: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        float f =
          col.R * 0.2627f +
          col.G * 0.6780f +
          col.B * 0.0593f;
        dst.fast_get<luma_t>(i) = f * mul;
      }
      break;
    }
    case desaturation_e::bt709: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        float f =
          col.R * 0.2126f +
          col.G * 0.7152f +
          col.B * 0.0722f;
        dst.fast_get<luma_t>(i) = f * mul;
      }
      break;
    }
    case desaturation_e::bt601: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        float f =
          col.R * 0.299f +
          col.G * 0.587f +
          col.B * 0.114f;
        dst.fast_get<luma_t>(i) = f * mul;
      }
      break;
    }
    case desaturation_e::hsl: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        auto a = std::max(col.R, col.G);
        a = std::max(a, col.B);
        auto b = std::min(col.R, col.G);
        b = std::min(b, col.B);
        dst.fast_get<luma_t>(i) = (a + b) * 0.5f * mul;
      }
      break;
    }
    case desaturation_e::euclidian_distance: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = std::sqrt(
          std::pow(col.R, 2.0f) +
          std::pow(col.G, 2.0f) +
          std::pow(col.B, 2.0f)
        ) * mul;
      }
      break;
    }
    case desaturation_e::red: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.R * mul;
      }
      break;
    }
    case desaturation_e::green: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.G * mul;
      }
      break;
    }
    case desaturation_e::blue: {
      #pragma omp parallel for simd
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.B * mul;
      }
      break;
    }
  } // switch type
} // desaturate

void gray_to_rgb24(CN(seze::Image) src, seze::Image &dst) {
  #pragma omp parallel for simd
  FOR (i, src.size) {
    auto l {src.fast_get<luma_t>(i)};
    l = std::clamp(l, 0.0f, 1.0f);
    l *= 255.0f;
    dst.fast_get<seze::RGB24>(i) = seze::RGB24(l, l, l);
  }
}

constexpr real blerp(real c00, real c10, real c01,
real c11, real tx, real ty)
  { return std::lerp(std::lerp(c00, c10, tx), std::lerp(c01, c11, tx), ty); }

constexpr real bcerp (real A, real B, real C, real D, real t) {
	return B + 0.5f * t * (C - A + t * (2.0f * A - 5.0f * B + 4.0f * C
    - D + t * (3.0f * (B - C) + D - A)));
}

static void scale_gray_bilinear(CN(seze::Image) src, seze::Image &dst) {
  real scale_x {1.0f / (real(dst.X) / src.X)};
  real scale_y {1.0f / (real(dst.Y) / src.Y)};
  #pragma omp parallel for simd
  FOR (y, dst.Y)
  FOR (x, dst.X) {
    real dx {x * scale_x};
    real dy {y * scale_y};
    int gxi (std::floor<int>(dx));
    int gyi (std::floor<int>(dy));
    auto c00 = src.fast_get<luma_t>(gxi,     gyi);
    auto c10 =      src.get<luma_t>(gxi + 1, gyi);
    auto c01 =      src.get<luma_t>(gxi,     gyi + 1);
    auto c11 =      src.get<luma_t>(gxi + 1, gyi + 1);
    auto tx = dx - gxi;
    auto ty = dy - gyi;
    auto l = blerp(c00, c10, c01, c11, tx, ty);
    dst.fast_set<luma_t>(x, y, l);
  }
} // scale_gray_bilinear

static void scale_gray_nearest(CN(seze::Image) src, seze::Image &dst) {
  real scale_x {1.0f / (real(dst.X) / src.X)};
  real scale_y {1.0f / (real(dst.Y) / src.Y)};
  #pragma omp parallel for simd
  FOR (y, dst.Y)
  FOR (x, dst.X) {
    real fx {x * scale_x};
    real fy {y * scale_y};
    auto src_col {src.fast_get<luma_t>(
      std::floor<int>(fx),
      std::floor<int>(fy))};
    dst.fast_set<luma_t>(x, y, src_col);
  }
} // scale_gray_nearest

static void scale_gray_bicubic(CN(seze::Image) src, seze::Image &dst) {
  real scale_x {1.0f / (real(dst.X) / src.X)};
  real scale_y {1.0f / (real(dst.Y) / src.Y)};
  #pragma omp parallel for simd
  FOR (y, dst.Y)
  FOR (x, dst.X) {
    real dx {x * scale_x};
    real dy {y * scale_y};
    int gxi (std::floor<int>(dx));
    int gyi (std::floor<int>(dy));
    double tx {dx - gxi};
    double ty {dy - gyi};
  // 16 points:
    // 1st row
    auto p00 = src.get<luma_t>(gxi - 1, gyi - 1);   
    auto p10 = src.get<luma_t>(gxi + 0, gyi - 1);   
    auto p20 = src.get<luma_t>(gxi + 1, gyi - 1);   
    auto p30 = src.get<luma_t>(gxi + 2, gyi - 1);   
    // 2nd row
    auto p01 = src.get<luma_t>(gxi - 1, gyi + 0);   
    auto p11 = src.get<luma_t>(gxi + 0, gyi + 0);   
    auto p21 = src.get<luma_t>(gxi + 1, gyi + 0);   
    auto p31 = src.get<luma_t>(gxi + 2, gyi + 0);
    // 3rd row
    auto p02 = src.get<luma_t>(gxi - 1, gyi + 1);   
    auto p12 = src.get<luma_t>(gxi + 0, gyi + 1);   
    auto p22 = src.get<luma_t>(gxi + 1, gyi + 1);   
    auto p32 = src.get<luma_t>(gxi + 2, gyi + 1);
    // 4th row
    auto p03 = src.get<luma_t>(gxi - 1, gyi + 2);   
    auto p13 = src.get<luma_t>(gxi + 0, gyi + 2);   
    auto p23 = src.get<luma_t>(gxi + 1, gyi + 2);   
    auto p33 = src.get<luma_t>(gxi + 2, gyi + 2);
  // interpolate bi-cubically:
    auto b0 = bcerp(p00, p10, p20, p30, tx);
    auto b1 = bcerp(p01, p11, p21, p31, tx);
    auto b2 = bcerp(p02, p12, p22, p32, tx);
    auto b3 = bcerp(p03, p13, p23, p33, tx);
    auto l = bcerp(b0, b1, b2, b3, ty);
    dst.fast_set(x, y, l);
  }
} // scale_gray_bicubic

static void scale_gray_bilinear_fast(CN(seze::Image) src, seze::Image &dst) {
  const real scale_x {1.0f / (real(dst.X) / src.X)};
  const real scale_y {1.0f / (real(dst.Y) / src.Y)};
  #pragma omp parallel for simd
  FOR (y, dst.Y - 1)
  FOR (x, dst.X - 1) {
    const real dx {x * scale_x};
    const int gxi = dx;
    const real tx = dx - gxi;
    const real dy {y * scale_y};
    const int gyi = dy;
    const real ty = dy - gyi;
    const auto c00 = src.fast_get<luma_t>(gxi,     gyi);
    const auto c10 = src.fast_get<luma_t>(gxi + 1, gyi);
    const auto c01 = src.fast_get<luma_t>(gxi,     gyi + 1);
    const auto c11 = src.fast_get<luma_t>(gxi + 1, gyi + 1);
    const auto l = blerp(c00, c10, c01, c11, tx, ty);
    dst.fast_set<luma_t>(x, y, l);
  }
} // scale_gray_bilinear_fast

static void scale_gray_bicubic_fast(CN(seze::Image) src, seze::Image &dst) {
  real scale_x {1.0f / (real(dst.X) / src.X)};
  real scale_y {1.0f / (real(dst.Y) / src.Y)};
  #pragma omp parallel for simd
  for (int y = 8; y < dst.Y - 8; ++y)
  for (int x = 8; x < dst.X - 8; ++x) {
    real dx {x * scale_x};
    real dy {y * scale_y};
    int gxi = dx;
    int gyi = dy;
    real tx {dx - gxi};
    real ty {dy - gyi};
  // 16 points:
    // 1st row
    auto p00 = src.fast_get<luma_t>(gxi - 1, gyi - 1);   
    auto p10 = src.fast_get<luma_t>(gxi + 0, gyi - 1);   
    auto p20 = src.fast_get<luma_t>(gxi + 1, gyi - 1);   
    auto p30 = src.fast_get<luma_t>(gxi + 2, gyi - 1);   
    // 2nd row
    auto p01 = src.fast_get<luma_t>(gxi - 1, gyi + 0);   
    auto p11 = src.fast_get<luma_t>(gxi + 0, gyi + 0);   
    auto p21 = src.fast_get<luma_t>(gxi + 1, gyi + 0);   
    auto p31 = src.fast_get<luma_t>(gxi + 2, gyi + 0);
    // 3rd row
    auto p02 = src.fast_get<luma_t>(gxi - 1, gyi + 1);   
    auto p12 = src.fast_get<luma_t>(gxi + 0, gyi + 1);   
    auto p22 = src.fast_get<luma_t>(gxi + 1, gyi + 1);   
    auto p32 = src.fast_get<luma_t>(gxi + 2, gyi + 1);
    // 4th row
    auto p03 = src.fast_get<luma_t>(gxi - 1, gyi + 2);   
    auto p13 = src.fast_get<luma_t>(gxi + 0, gyi + 2);   
    auto p23 = src.fast_get<luma_t>(gxi + 1, gyi + 2);   
    auto p33 = src.fast_get<luma_t>(gxi + 2, gyi + 2);
  // interpolate bi-cubically:
    auto b0 = bcerp(p00, p10, p20, p30, tx);
    auto b1 = bcerp(p01, p11, p21, p31, tx);
    auto b2 = bcerp(p02, p12, p22, p32, tx);
    auto b3 = bcerp(p03, p13, p23, p33, tx);
    auto l = bcerp(b0, b1, b2, b3, ty);
    dst.fast_set(x, y, l);
  }
} // scale_gray_bicubic_fast

void scale_gray(CN(seze::Image) src, seze::Image &dst, scale_e type) {
  switch (type) {
    default:
    case scale_e::bilinear: scale_gray_bilinear(src, dst); break;
    case scale_e::nearest: scale_gray_nearest(src, dst); break;
    case scale_e::bicubic: scale_gray_bicubic(src, dst); break;
    case scale_e::bilinear_fast: scale_gray_bilinear_fast(src, dst); break;
    case scale_e::bicubic_fast: scale_gray_bicubic_fast(src, dst); break;
  }
} // scale_gray

static void filter_average(v_luma_t &stream, int power = 3) {
  return_if (power < 1);
  const real power_mul {1.0f / power};
  const int power_mid {power / 2};
  v_luma_t buf(stream.size());
  FOR (i, stream.size()) {
    luma_t total {0};
    FOR (wnd, power) {
      try {
        total += stream.at(i + wnd - power_mid);
      } catch (...) {}
    }
    buf[i] = total * power_mul;
  }
  stream = buf;
} // filter_average

static void filter_average_fast(v_luma_t &stream, int power = 3) {
  return_if (power < 1);
  const real power_mul {1.0f / power};
  FOR (i, stream.size() - power) {
    luma_t total {0};
    FOR (wnd, power)
      total += stream[i + wnd];
    stream[i] = total * power_mul;
  }
}

static void filter_median(v_luma_t&stream, int power = 3) {
  return_if (power < 1);
  const int power_mid {power / 2};
  v_luma_t arr(power);
  v_luma_t buf(stream.size());
  FOR (i, stream.size()) {
    FOR (wnd, power) {
      try {
        arr[wnd] = stream.at(i + wnd - power_mid);
      } catch (...) {}
    }
    std::sort(arr.begin(), arr.end());
    buf[i] = arr[power_mid];
  }
  stream = buf;
} // filter_median

void filtering(v_luma_t &stream, int power, filter_e type) {
  switch (type) {
    default:
    case filter_e::none: break;
    case filter_e::average: filter_average(stream, power); break;
    case filter_e::average_fast: filter_average_fast(stream, power); break;
    case filter_e::median: filter_median(stream, power); break;
  }
}

void apply_noise(v_luma_t &stream, real noise_level) {
  return_if(noise_level <= 0);
  #pragma omp for simd
  FOR (i, stream.size())
    stream[i] += noise_level * seze::frand_fast();
}

seze::YUVf RGB24_to_yuv(CN(seze::RGB24) c) {
  constexpr auto mul {1.0f / 255.0f};
  auto fr {c.R * mul};
	auto fg {c.G * mul};
	auto fb {c.B * mul};
	auto Y = 0.2989f * fr + 0.5866f * fg + 0.1145f * fb;
	auto Cb = -0.1687f * fr - 0.3313f * fg + 0.5000f * fb;
	auto Cr = 0.5000f * fr - 0.4184f * fg - 0.0816f * fb;
  return seze::YUVf(Y, Cb, Cr);
}

 seze::RGB24 yuv_to_RGB24(CN(seze::YUVf) c) {
  constexpr auto mul {255.0f};
  auto r = std::clamp(c.Y + 0.0000f * c.U + 1.4022f * c.V, 0.0f, 1.0f);
  r *= mul;
	auto g = std::clamp(c.Y - 0.3456f * c.U - 0.7145f * c.V, 0.0f, 1.0f);
  g *= mul;
	auto b = std::clamp(c.Y + 1.7710f * c.U + 0.0000f * c.V, 0.0f, 1.0f);
  b *= mul;
	return seze::RGB24(r, g, b);
}
