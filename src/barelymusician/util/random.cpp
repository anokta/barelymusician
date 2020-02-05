#include "barelymusician/util/random.h"

namespace barelyapi {

std::default_random_engine Random::generator_ = std::default_random_engine();

std::normal_distribution<double> Random::normal_double_distribution_ =
    std::normal_distribution<double>();

std::normal_distribution<float> Random::normal_float_distribution_ =
    std::normal_distribution<float>();

std::uniform_real_distribution<double> Random::uniform_double_distribution_ =
    std::uniform_real_distribution<double>();

std::uniform_real_distribution<float> Random::uniform_float_distribution_ =
    std::uniform_real_distribution<float>();

std::uniform_int_distribution<int> Random::uniform_int_distribution_ =
    std::uniform_int_distribution<int>();

double Random::Normal(double mean, double variance) {
  std::normal_distribution<double>::param_type param(mean, variance);
  return normal_double_distribution_(generator_, param);
}

float Random::Normal(float mean, float variance) {
  std::normal_distribution<float>::param_type param(mean, variance);
  return normal_float_distribution_(generator_, param);
}

void Random::SetSeed(int seed) {
  generator_.seed(static_cast<unsigned int>(seed));
}

double Random::Uniform(double min, double max) {
  std::uniform_real_distribution<double>::param_type param(min, max);
  return uniform_double_distribution_(generator_, param);
}

float Random::Uniform(float min, float max) {
  std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_float_distribution_(generator_, param);
}

int Random::Uniform(int min, int max) {
  std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(generator_, param);
}

}  // namespace barelyapi
