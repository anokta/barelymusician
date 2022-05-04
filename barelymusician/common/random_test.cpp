#include "barelymusician/common/random.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the generated uniform numbers are always within a given range.
TEST(RandomTest, DrawUniformMinMax) {
  const int kNumValues = 1000;
  const int kMin = -7;
  const int kMax = 35;

  Random random;
  for (int i = 0; i < kNumValues; ++i) {
    const int value = random.DrawUniform(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LE(value, kMax);
  }
}

// Tests that the same values are generated when reset with the same seed.
TEST(RandomTest, Reset) {
  const int kSeed = 1;
  const int kNumValues = 10;
  const double kMean = -0.5;
  const double kVariance = 10.0;

  Random random(kSeed);

  // Generate some random values.
  std::vector<double> values(kNumValues);
  for (int i = 0; i < kNumValues; ++i) {
    values[i] = random.DrawNormal(kMean, kVariance);
  }

  // Reset the seed with the same value.
  random.Reset(kSeed);
  // Validate that the same numbers are generated for the next `kNumValues`.
  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_DOUBLE_EQ(random.DrawNormal(kMean, kVariance), values[i]);
  }
}

}  // namespace
}  // namespace barely
