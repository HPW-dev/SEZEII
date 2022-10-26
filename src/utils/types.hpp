#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <memory>

using ldouble = long double;
using uint = unsigned int;
using byte = uint8_t;
using Real = float;
using Str = std::string;

template <class T>
using shared_p = std::shared_ptr<T>;

template <class T, typename... Args>
shared_p<T> make_shared_p(Args&&... args)
  { return std::make_shared<T>(args...); }

template <class T, class alloc = std::allocator<T>>
using vector_t = std::vector<T, alloc>;

using v_byte_t = vector_t<byte>;
using v_str_t = vector_t<Str>;
