#pragma once
#include <chrono>
#include <thread>
#include "macro.hpp"
#include "types.hpp"

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

TimePoint get_time();
real time_diff_sec(CN(TimePoint) t1, CN(TimePoint) t2);

template <class T>
void sleep(CN(T) t) { std::this_thread::sleep_for(t); }
