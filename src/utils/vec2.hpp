#pragma once
#include "utils/macro.hpp"

template <class T>
struct vec2_templ {
  T x {0}, y {0};
  
  constexpr vec2_templ(T x_=0, T y_=0): x{x_}, y{y_} {}

  template <class T2>
  constexpr vec2_templ(const vec2_templ<T2> other) 
  : x {other.x}
  , y {other.y}
  {}
};

using vec2i = vec2_templ<int>;
using vec2u = vec2_templ<uint>;
using vec2f = vec2_templ<float>;
