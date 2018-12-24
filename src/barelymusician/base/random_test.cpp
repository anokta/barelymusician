#include "barelymusician/base/random.h"

#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace random {
namespace {

// Tests that the random number generator generates the same values when it is
// reset with the same seed.
TEST(RandomTest, Reset) {
  const int kSeed = 1;
  const int kNumValues = 10;

  // Set the seed.
  Reset(kSeed);
  // Generate some random values.
  std::vector<float> values(kNumValues);
  for (int i = 0; i < kNumValues; ++i) {
    values[i] = Uniform();
  }
  // Reset the seed with the same value.
  Reset(kSeed);

  // Validate that the same numbers are generated for the next |kNumValues|.
  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_FLOAT_EQ(values[i], Uniform());
  }
}

// Tests that the uniform random number generation always returns a value within
// the given range.
TEST(RandomTest, UniformMinMax) {
  const int kNumValues = 1000;
  const int kMin = -7;
  const int kMax = 35;

  for (int i = 0; i < kNumValues; ++i) {
    const int value = Uniform(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LE(value, kMax);
  }
}

}  // namespace
}  // namespace random
}  // namespace barelyapi
