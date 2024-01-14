#include "barelymusician/common/random.h"

#include <cassert>
#include <cstdint>
#include <random>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"

namespace barely {

Random::Random(int seed) noexcept : engine_(static_cast<unsigned int>(seed)) {}

double Random::DrawNormal(double mean, double variance) noexcept {
  assert(variance >= 0.0);
  const std::normal_distribution<double>::param_type param(mean, variance);
  return normal_distribution_(engine_, param);
}

double Random::DrawUniform(double min, double max) noexcept {
  assert(min <= max);
  const std::uniform_real_distribution<double>::param_type param(min, max);
  return uniform_real_distribution_(engine_, param);
}

int Random::DrawUniform(int min, int max) noexcept {
  assert(min <= max);
  const std::uniform_int_distribution<int>::param_type param(min, max);
  return uniform_int_distribution_(engine_, param);
}

Rational Random::DrawUniform(Rational min, Rational max) noexcept {
  assert(min <= max);
  std::int64_t min_numerator = min.numerator;
  std::int64_t max_numerator = max.numerator;
  std::int64_t denominator = min.denominator;
  if (min.denominator != max.denominator) {
    min_numerator *= max.denominator;
    max_numerator *= min.denominator;
    denominator *= max.denominator;
  }
  const std::uniform_int_distribution<std::int64_t>::param_type param(min_numerator, max_numerator);
  return RationalNormalized(uniform_rational_distribution_(engine_, param), denominator);
}

void Random::Reset(int seed) noexcept {
  assert(seed >= 0);
  engine_.seed(static_cast<unsigned int>(seed));
}

}  // namespace barely
