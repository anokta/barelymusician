#include "barelymusician/dsp/decibels.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that amplitude/decibels conversion returns expected results.
TEST(DecibelsTest, AmplitudeDecibelsConversion) {
  constexpr double kEpsilon = 5e-2;

  constexpr int kValueCount = 4;
  constexpr std::array<double, kValueCount> kAmplitudes = {0.0, 0.1, 1.0, 2.0};
  constexpr std::array<double, kValueCount> kDecibels = {-80.0, -20.0, 0.0, 6.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_NEAR(AmplitudeFromDecibels(kDecibels[i]), kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(kAmplitudes[i]), kDecibels[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])), kAmplitudes[i]);
    EXPECT_DOUBLE_EQ(DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])), kDecibels[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_DOUBLE_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0);
  EXPECT_DOUBLE_EQ(DecibelsFromAmplitude(0.0), kMinDecibels);
}

}  // namespace
}  // namespace barely
