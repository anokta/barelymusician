#include "barelymusician/dsp/decibels.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that amplitude/decibels conversion returns expected results.
TEST(DecibelsTest, AmplitudeDecibelsConversion) {
  constexpr float kEpsilon = 5e-2f;

  constexpr int kValueCount = 4;
  constexpr std::array<float, kValueCount> kAmplitudes = {0.0f, 0.1f, 1.0f, 2.0f};
  constexpr std::array<float, kValueCount> kDecibels = {-80.0f, -20.0f, 0.0f, 6.0f};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_NEAR(AmplitudeFromDecibels(kDecibels[i]), kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(kAmplitudes[i]), kDecibels[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_FLOAT_EQ(AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])), kAmplitudes[i]);
    EXPECT_FLOAT_EQ(DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])), kDecibels[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0f);
  EXPECT_FLOAT_EQ(DecibelsFromAmplitude(0.0f), kMinDecibels);
}

}  // namespace
}  // namespace barely
