#include "barelymusician/util/random.h"

#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace random {
namespace {

// Tests that the random number generator generates the same values when it is
// reset with the same seed.
TEST(RandomTest, SetSeed) {
  const int kSeed = 1;
  const int kNumValues = 10;
  const float kMean = -0.5f;
  const float kVariance = 10.0f;

  // Set the seed.
  Random::SetSeed(kSeed);
  // Generate some random values.
  std::vector<float> values(kNumValues);
  for (int i = 0; i < kNumValues; ++i) {
    values[i] = Random::Normal(kMean, kVariance);
  }

  // Reset the seed with the same value.
  Random::SetSeed(kSeed);
  // Validate that the same numbers are generated for the next |kNumValues|.
  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_FLOAT_EQ(Random::Normal(kMean, kVariance), values[i]);
  }
}

// Tests that the uniform random number generation always returns a value within
// the given range.
TEST(RandomTest, UniformMinMax) {
  const int kNumValues = 1000;
  const int kMin = -7;
  const int kMax = 35;

  for (int i = 0; i < kNumValues; ++i) {
    const int value = Random::Uniform(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LE(value, kMax);
  }
}

}  // namespace
}  // namespace random
}  // namespace barelyapi
