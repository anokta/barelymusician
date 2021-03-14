#include "barelymusician/common/random_generator.h"

#include <vector>

#include "gtest/gtest.h"

namespace barelyapi::random {
namespace {

// Tests that the uniform random generation always returns a value within the
// given range.
TEST(RandomGeneratorTest, DrawUniformMinMax) {
  const int kNumValues = 1000;
  const int kMin = -7;
  const int kMax = 35;

  RandomGenerator random_generator;
  for (int i = 0; i < kNumValues; ++i) {
    const int value = random_generator.DrawUniform(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LE(value, kMax);
  }
}

// Tests that the random generator returns the same values when it is reset with
// the same seed.
TEST(RandomGeneratorTest, Reset) {
  const int kSeed = 1;
  const int kNumValues = 10;
  const float kMean = -0.5f;
  const float kVariance = 10.0f;

  RandomGenerator random_generator(kSeed);

  // Generate some random values.
  std::vector<float> values(kNumValues);
  for (int i = 0; i < kNumValues; ++i) {
    values[i] = random_generator.DrawNormal(kMean, kVariance);
  }

  // Reset the seed with the same value.
  random_generator.Reset(kSeed);
  // Validate that the same numbers are generated for the next |kNumValues|.
  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_FLOAT_EQ(random_generator.DrawNormal(kMean, kVariance), values[i]);
  }
}

}  // namespace
}  // namespace barelyapi::random
