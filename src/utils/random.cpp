#include "random.hpp"
#include <ctime>

namespace seze {

std::mutex random_mu = {};
std::mt19937 generator = {};
int rseed = 0;

//! быстрый хороший рандом (не потокобезопасный)
uint32_t lcg_parkmiller() {
  static uint32_t state = rseed;
  // Precomputed parameters for Schrage's method
  constexpr uint32_t M = 0x7fffffff;
  constexpr uint32_t A = 48271;
  constexpr uint32_t Q = M / A;    // 44488
  constexpr uint32_t R = M % A;    //  3399
  uint32_t div = state / Q;	// max: M / Q = A = 48,271
  uint32_t rem = state % Q;	// max: Q - 1     = 44,487
  int32_t s = rem * A;	// max: 44,487 * 48,271 = 2,147,431,977 = 0x7fff3629
  int32_t t = div * R;	// max: 48,271 *  3,399 =   164,073,129
  int32_t result = s - t;
  if (result < 0)
    result += M;
  return state = result;
}

int irand_fast() { return lcg_parkmiller(); }

int irand(int min_, int max_) {
  std::lock_guard<std::mutex> lock(random_mu);
  std::uniform_int_distribution<int>
    distribution(min_, max_);
  return distribution(generator);
}

ldouble ldrand(ldouble min_, ldouble max_) {
  std::lock_guard<std::mutex> lock(random_mu);
  std::uniform_real_distribution<ldouble>
    distribution(min_, max_);
  return distribution(generator);
}

double drand(double min_, double max_) {
  std::lock_guard<std::mutex> lock(random_mu);
  std::uniform_real_distribution<double>
    distribution(min_, max_);
  return distribution(generator);
}

float frand(float min_, float max_) {
  std::lock_guard<std::mutex> lock(random_mu);
  std::uniform_real_distribution<float>
    distribution(min_, max_);
  return distribution(generator);
}

void randomize_seed() {
  std::lock_guard<std::mutex> lock(random_mu);
  /*std::random_device r;
  std::seed_seq seed2 {
    r(), r(), r(), r(),
    r(), r(), r(), r() };*/
  rseed = time(nullptr);
  std::seed_seq seed2 { rseed };
  generator = std::mt19937(seed2);
}

} // seze ns
