#pragma once
#include <memory>

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

#ifndef ALIGN
#define ALIGN 64
#endif

//! макрос для классов запрещающий копирование
#define nocopy(class_name) \
class_name(CN(class_name)) = delete; \
class_name& operator = (CN(class_name)) = delete; \
class_name(class_name&&) = delete; \
class_name& operator = (class_name&&) = delete;
