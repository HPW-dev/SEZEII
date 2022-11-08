#pragma once
#include "macro.hpp"
#include <iostream>
#include <mutex>
namespace seze { extern std::mutex log_mu; }
#define LOG(msg) { \
  std::lock_guard<std::mutex> lock(seze::log_mu); \
  std::clog << msg; \
  std::clog.flush(); \
}
