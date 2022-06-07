#pragma once
#include <memory>
#include <cstdint>

using ldouble = long double;
using uint = unsigned int;
using byte = uint8_t;

#define scast(T, X) static_cast<T>(X)
#define rcast(T, X) reinterpret_cast<T>(X)
#define dcast(T, X) dinamyc_cast<T>(X)
#define CN(T) const T&
#define CP(T) const T*

#define FOR(name, mx) \
for (auto name = decltype(mx){0}; name < mx; ++name)
template <typename T> void zero_delete(T& p) {
  delete p;
  p = nullptr;
}

template <class T>
using shared_p = std::shared_ptr<T>;

template <class T, typename... Args>
inline shared_p<T> make_shared_p(Args&&... args)
  { return std::make_shared<T>(args...); }

#ifndef ALIGN
#define ALIGN 64
#endif
