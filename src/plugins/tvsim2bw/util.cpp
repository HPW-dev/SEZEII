#include <cmath>
#include "util.hpp"
#include "image/image.hpp"
#include "image/rgb24.hpp"

void desaturate(CN(seze::Image) src, seze::Image &dst,
desaturation_e type) {
  auto mul {1.0f / 255.0f};
  switch (type) {
    default:
    case desaturation_e::average: {
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        auto frgb {(col.R + col.G + col.B) / 3.0f};
        dst.fast_get<luma_t>(i) = frgb * mul;
      }
      break;
    }
    case desaturation_e::bt2001: {
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
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.R * mul;
      }
      break;
    }
    case desaturation_e::green: {
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.G * mul;
      }
      break;
    }
    case desaturation_e::blue: {
      FOR (i, src.size) {
        auto col {src.fast_get<seze::RGB24>(i)};
        dst.fast_get<luma_t>(i) = col.B * mul;
      }
      break;
    }
  } // switch type
} // desaturate

void gray_to_rgb24(CN(seze::Image) src, seze::Image &dst) {
  FOR (i, src.size) {
    auto l {src.fast_get<luma_t>(i)};
    l = std::clamp(l, 0.0f, 1.0f);
    l *= 255.0f;
    dst.fast_get<seze::RGB24>(i) = seze::RGB24(l, l, l);
  }
}

void scale_gray_bilinear(CN(seze::Image) src, seze::Image &dst) {

}

void scale_gray_nearest(CN(seze::Image) src, seze::Image &dst) {

}

void scale_gray_bicubic(CN(seze::Image) src, seze::Image &dst) {

}

void scale_gray(CN(seze::Image) src, seze::Image &dst, scale_e type) {
  switch (type) {
    default:
    case scale_e::bilinear: scale_gray_bilinear(src, dst); break;
    case scale_e::nearest: scale_gray_nearest(src, dst); break;
    case scale_e::bicubic: scale_gray_bicubic(src, dst); break;
  }
} // scale_gray
