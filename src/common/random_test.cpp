#include "common/random.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the generated uniform numbers are always within a given range.
TEST(RandomTest, DrawUniformMinMax) {
  constexpr int kValueCount = 1000;
  constexpr int kMin = -7;
  constexpr int kMax = 35;

  Random random;
  for (int i = 0; i < kValueCount; ++i) {
    const int value = random.DrawUniform(kMin, kMax);
    EXPECT_GE(value, kMin);
    EXPECT_LT(value, kMax);
  }
}

// Tests that the same values are generated when reset with the same seed.
TEST(RandomTest, Reset) {
  constexpr int kSeed = 1;
  constexpr int kValueCount = 10;
  constexpr double kMean = -0.5;
  constexpr double kVariance = 10.0;

  Random random(kSeed);

  // Generate some random values.
  std::vector<double> values(kValueCount);
  for (int i = 0; i < kValueCount; ++i) {
    values[i] = random.DrawNormal(kMean, kVariance);
  }

  // Reset the seed with the same value.
  random.Reset(kSeed);
  // Validate that the same numbers are generated for the next `kValueCount`.
  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(random.DrawNormal(kMean, kVariance), values[i]);
  }
}

}  // namespace
}  // namespace barely::internal
