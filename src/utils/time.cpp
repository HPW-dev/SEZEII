#include "time.hpp"

TimePoint get_time() { return std::chrono::high_resolution_clock::now(); }

real time_diff_sec(CN(TimePoint) t1, CN(TimePoint) t2) {
  using Sec = std::chrono::duration<real>;
  Sec sec = t1 - t2;
  return sec.count();
}
