#include "common/random_impl.h"

#include <cassert>
#include <random>

namespace barely::internal {

RandomImpl::RandomImpl(int seed) noexcept : engine_(static_cast<unsigned int>(seed)) {}

float RandomImpl::DrawNormal(float mean, float variance) noexcept {
  assert(variance > 0.0);
  const std::normal_distribution<float>::param_type param(mean, variance);
  return normal_distribution_(engine_, param);
}

float RandomImpl::DrawUniform(float min, float max) noexcept {
  assert(min < max);
  const std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_real_distribution_(engine_, param);
}

int RandomImpl::DrawUniform(int min, int max) noexcept {
  assert(min < max);
  const std::uniform_int_distribution<int>::param_type param(min, max - 1);
  return uniform_int_distribution_(engine_, param);
}

void RandomImpl::Reset(int seed) noexcept {
  assert(seed >= 0);
  engine_.seed(static_cast<unsigned int>(seed));
}

}  // namespace barely::internal
