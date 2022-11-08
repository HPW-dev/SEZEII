#pragma once
#include <stdexcept>
#include <mutex>
#include <iostream>

namespace {
static std::recursive_mutex cerr_mu = {};
}

template <class T>
void error(T msg) {
  cerr_mu.lock();
  std::cerr << std::endl << "[ERROR] " << msg << std::endl;
  cerr_mu.unlock();
  std::terminate();
}

template <class T>
void iferror(bool cond, T msg) {
  if (cond)
    error(msg);
}
