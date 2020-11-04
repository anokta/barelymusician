#include "barelymusician/base/random.h"

namespace barelyapi {
namespace random {

namespace {

// Pseudo-random number generator engine.
std::default_random_engine generator;

}  // namespace

double Normal(double mean, double variance) {
  static std::normal_distribution<double> normal_double_distribution;
  std::normal_distribution<double>::param_type param(mean, variance);
  return normal_double_distribution(generator, param);
}

float Normal(float mean, float variance) {
  static std::normal_distribution<float> normal_float_distribution;
  std::normal_distribution<float>::param_type param(mean, variance);
  return normal_float_distribution(generator, param);
}

void SetSeed(int seed) { generator.seed(static_cast<unsigned int>(seed)); }

double Uniform(double min, double max) {
  static std::uniform_real_distribution<double> uniform_double_distribution;
  std::uniform_real_distribution<double>::param_type param(min, max);
  return uniform_double_distribution(generator, param);
}

float Uniform(float min, float max) {
  static std::uniform_real_distribution<float> uniform_float_distribution;
  std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_float_distribution(generator, param);
}

int Uniform(int min, int max) {
  static std::uniform_int_distribution<int> uniform_int_distribution;
  std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution(generator, param);
}

}  // namespace random
}  // namespace barelyapi
