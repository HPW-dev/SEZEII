#pragma once
///@file thread safe random gen
#include "macro.hpp"
#include <mutex>
#include <random>

namespace seze {

extern std::mutex random_mu;
extern std::mt19937 generator;

int irand(int min_=0, int max_=RAND_MAX);
int irand_fast();
ldouble ldrand(ldouble min_=0, ldouble max_=1);
double drand(double min_=0, double max_=1);
float frand(float min_=0.0f, float max_=1.0f);
void randomize_seed();

} // seze ns
