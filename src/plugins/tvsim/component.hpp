#pragma once
#include "../../image/color.hpp"
#include "../../image/YUV.hpp"
#include <itlib/pod_vector.hpp>
/** @file
 * TVSIM_LD_COMPONENT - long double gray
 * TVSIM_FLOAT_COMPONENT - float gray
 */
#ifndef TVSIM_LD_COMPONENT
#define TVSIM_FLOAT_COMPONENT
#endif

namespace {
#ifdef TVSIM_LD_COMPONENT
using component = long double;
using component_yuv = seze::YUVld;
auto component_type = seze::color_t::ld_gray;
auto component_yuv_type = seze::color_t::YUVld;
#endif
#ifdef TVSIM_FLOAT_COMPONENT
using component = float;
using component_yuv = seze::YUVf;
auto component_type = seze::color_t::f_gray;
auto component_yuv_type = seze::color_t::YUVf;
#endif
}

//! component stream
struct Stream {
  int size = 0;
  itlib::pod_vector<component> data = {};
};
