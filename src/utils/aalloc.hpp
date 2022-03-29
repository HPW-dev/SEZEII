#pragma once
#include <cstddef>
#include "macro.hpp"

void* aalloc(size_t aling, size_t size);
void afree(void* data);
