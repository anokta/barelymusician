#include "barelymusician/base/random.h"

namespace barelyapi {

std::default_random_engine Random::generator_ = std::default_random_engine();

std::normal_distribution<float> Random::normal_distribution_ =
    std::normal_distribution<float>();

std::uniform_real_distribution<float> Random::uniform_float_distribution_ =
    std::uniform_real_distribution<float>();

std::uniform_int_distribution<int> Random::uniform_int_distribution_ =
    std::uniform_int_distribution<int>();

float Random::Normal(float mean, float variance) {
  std::normal_distribution<float>::param_type param(mean, variance);
  return normal_distribution_(generator_, param);
}

void Random::SetSeed(int seed) {
  generator_.seed(static_cast<unsigned int>(seed));
}

float Random::Uniform() { return Uniform(0.0f, 1.0f); }

float Random::Uniform(float min, float max) {
  std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_float_distribution_(generator_, param);
}

int Random::Uniform(int min, int max) {
  std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(generator_, param);
}

}  // namespace barelyapi
