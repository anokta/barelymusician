#include "barelymusician/common/random.h"

#include <cassert>
#include <random>

namespace barely {

Random::Random(int seed) noexcept : engine_(static_cast<unsigned int>(seed)) {}

float Random::DrawNormal(float mean, float variance) noexcept {
  assert(variance >= 0.0);
  const std::normal_distribution<float>::param_type param(mean, variance);
  return normal_distribution_(engine_, param);
}

float Random::DrawUniform(float min, float max) noexcept {
  assert(min <= max);
  const std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_real_distribution_(engine_, param);
}

int Random::DrawUniform(int min, int max) noexcept {
  assert(min <= max);
  const std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(engine_, param);
}

void Random::Reset(int seed) noexcept {
  assert(seed >= 0);
  engine_.seed(static_cast<unsigned int>(seed));
}

}  // namespace barely
