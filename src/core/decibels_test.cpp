#include "core/decibels.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(DecibelsTest, AmplitudeDecibelsConversion) {
  constexpr double kEpsilon = 5e-2;

  constexpr int kValueCount = 4;
  constexpr std::array<double, kValueCount> kAmplitudes = {0.0, 0.1, 1.0, 2.0};
  constexpr std::array<double, kValueCount> kDecibels = {-60.0, -20.0, 0.0, 6.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_NEAR(AmplitudeToDecibels(kAmplitudes[i]), kDecibels[i], kEpsilon);
    EXPECT_NEAR(DecibelsToAmplitude(kDecibels[i]), kAmplitudes[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(AmplitudeToDecibels(DecibelsToAmplitude(kDecibels[i])), kDecibels[i]);
    EXPECT_DOUBLE_EQ(DecibelsToAmplitude(AmplitudeToDecibels(kAmplitudes[i])), kAmplitudes[i]);
  }
}

TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_DOUBLE_EQ(AmplitudeToDecibels(0.0), kMinDecibels);
  EXPECT_DOUBLE_EQ(DecibelsToAmplitude(kMinDecibels), 0.0);
}

}  // namespace
}  // namespace barely
