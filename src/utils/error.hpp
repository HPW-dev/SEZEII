#pragma once
#include <stdexcept>
#define error(msg) throw std::runtime_error(msg);
#define iferror(cond, msg) { \
if (cond) \
  error(msg) \
}
