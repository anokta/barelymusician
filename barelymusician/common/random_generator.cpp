#include "barelymusician/common/random_generator.h"

namespace barelyapi {

RandomGenerator::RandomGenerator(int seed)
    : engine_(static_cast<unsigned int>(seed)) {}

double RandomGenerator::DrawNormal(double mean, double variance) {
  std::normal_distribution<double>::param_type param(mean, variance);
  return normal_double_distribution_(engine_, param);
}

float RandomGenerator::DrawNormal(float mean, float variance) {
  std::normal_distribution<float>::param_type param(mean, variance);
  return normal_float_distribution_(engine_, param);
}

double RandomGenerator::DrawUniform(double min, double max) {
  std::uniform_real_distribution<double>::param_type param(min, max);
  return uniform_double_distribution_(engine_, param);
}

float RandomGenerator::DrawUniform(float min, float max) {
  std::uniform_real_distribution<float>::param_type param(min, max);
  return uniform_float_distribution_(engine_, param);
}

int RandomGenerator::DrawUniform(int min, int max) {
  std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(engine_, param);
}

void RandomGenerator::Reset(int seed) {
  engine_.seed(static_cast<unsigned int>(seed));
}

}  // namespace barelyapi
