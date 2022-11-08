#pragma once
#include "vec2.hpp"

template <class T>
struct rect_templ {
  using vec_t = vec2_templ<T>;
  vec_t pos {};
  vec_t size {};

  rect_templ() = default;

  rect_templ(CN(vec_t) pos_, CN(vec_t) size_)
  : pos(pos_)
  , size(size_)
  {}

  template <class T2>
  constexpr rect_templ(const rect_templ<T2> other) 
  : pos{other.pos}
  , size{other.size}
  {}
};

using irect = rect_templ<int>;
using urect = rect_templ<uint>;
using frect = rect_templ<float>;
