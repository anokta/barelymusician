#include "barelymusician/common/random.h"

#include <random>

namespace barely {

Random::Random(int seed) : engine_(static_cast<unsigned int>(seed)) {}

double Random::DrawNormal(double mean, double variance) {
  std::normal_distribution<double>::param_type param(mean, variance);
  return normal_double_distribution_(engine_, param);
}

float Random::DrawNormal(float mean, float variance) {
  std::normal_distribution<float>::param_type param(mean, variance);
  return normal_float_distribution_(engine_, param);
}

double Random::DrawUniform(double min, double max) {
  std::uniform_real_distribution<double>::param_type param(min, max);
  return uniform_double_distribution_(engine_, param);
}

float Random::DrawUniform(float min, float max) {
  std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_float_distribution_(engine_, param);
}

int Random::DrawUniform(int min, int max) {
  std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(engine_, param);
}

void Random::Reset(int seed) { engine_.seed(static_cast<unsigned int>(seed)); }

}  // namespace barely
