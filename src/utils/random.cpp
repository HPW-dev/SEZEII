#include <ctime>
#include <atomic>
#include "random.hpp"
#include "table_rand.inc"

namespace seze {

std::mutex random_mu = {};
generator_t generator = {};
std::atomic_int rseed(1);

//! быстрый хороший рандом (не потокобезопасный)
static uint32_t lcg_parkmiller() {
  static std::atomic<uint32_t> state(rseed);
  // Precomputed parameters for Schrage's method
  constexpr uint32_t M = 0x7fffffff;
  constexpr uint32_t A = 48271;
  constexpr uint32_t Q = M / A;    // 44488
  constexpr uint32_t R = M % A;    //  3399
  uint32_t div = state / Q;	// max: M / Q = A = 48,271
  uint32_t rem = state % Q;	// max: Q - 1     = 44,487
  int32_t s = rem * A;	// max: 44,487 * 48,271 = 2,147,431,977 = 0x7fff3629
  int32_t t = div * R;	// max: 48,271 *  3,399 = 164,073,129
  int32_t result = s - t;
  if (result < 0)
    result += M;
  return state = result;
}

static uint8_t z26_random() {
  static std::atomic<uint8_t> r_seed(rseed % 256);
  r_seed = 
    (((((r_seed & 0x80) >> 7)   ^
       ((r_seed & 0x20) >> 5))  ^
      (((r_seed & 0x10) >> 4)   ^
       ((r_seed & 0x08) >> 3))) ^ 1) |
        (r_seed         << 1);
  return r_seed;
}

static uint RPNG() {
  static std::atomic<uint> r_seed(rseed % 0x7FFF);
  r_seed = 8253729 * r_seed + 2396403;
  return r_seed % 32767;
}

static uint8_t get_table_rand() {
  static std::atomic<uint16_t> r_seed(rseed);
  return table_rand[r_seed++];
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

float frand_fast(float min_, float max_) {
  auto total = max_ + std::abs(min_);
  constexpr auto rnd_max = 256;
  auto raw = float(get_table_rand()) / float(rnd_max);
  return min_ + raw * total;
}

void randomize_seed() {
  std::lock_guard<std::mutex> lock(random_mu);
  /*std::random_device r;
  std::seed_seq seed2 {
    r(), r(), r(), r(),
    r(), r(), r(), r() };*/
  rseed = time(nullptr);
  std::seed_seq seed2 { int(rseed) };
  generator = decltype(generator)(seed2);
}

} // seze ns
