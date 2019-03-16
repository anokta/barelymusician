#include "barelymusician/base/random.h"

#include <random>

namespace barelyapi {
namespace random {

namespace {

// TODO(#27): Global initialization should be discouraged, revisit how the
// generator is initialized - along with the static distribution declarations
// below.
// Random number generator engine.
static std::default_random_engine generator = std::default_random_engine();

}  // namespace

float Normal(float mean, float variance) {
  static std::normal_distribution<float> distribution;
  std::normal_distribution<float>::param_type param(mean, variance);
  return distribution(generator, param);
}

void Reset(int seed) { generator.seed(static_cast<unsigned int>(seed)); }

float Uniform() { return Uniform(0.0f, 1.0f); }

float Uniform(float min, float max) {
  static ::std::uniform_real_distribution<float> distribution;
  std::uniform_real_distribution<float>::param_type param(min, max);
  return distribution(generator, param);
}

int Uniform(int min, int max) {
  static std::uniform_int_distribution<int> distribution;
  std::uniform_int_distribution<int>::param_type param(min, max);
  return distribution(generator, param);
}

}  // namespace random
}  // namespace barelyapi
