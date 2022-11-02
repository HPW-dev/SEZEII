#pragma once
#include <cstddef>
#include "macro.hpp"

void* aalloc(size_t aling, size_t size);
void afree(void* data);

//! для выровненный аллокатор для std
template <class T>
class Aallocator {
public:
  using value_type = T;

  T* allocate(std::size_t size)
    { return rcast(T*, aalloc(64, size * sizeof(T)) ); }

  void deallocate(T* p, std::size_t /*size*/)
    { afree(p); }
}; // Aallocator

template <class T1, class T2>
constexpr bool operator== (const Aallocator<T1> &a,
const Aallocator<T2> &b) noexcept {
  return typeid(decltype(a)) == typeid(decltype(b));
}
