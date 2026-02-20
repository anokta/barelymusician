#include "core/decibels.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(DecibelsTest, AmplitudeDecibelsConversion) {
  constexpr float kEpsilon = 5e-2f;

  constexpr int kValueCount = 4;
  constexpr std::array<float, kValueCount> kAmplitudes = {0.0f, 0.1f, 1.0f, 2.0f};
  constexpr std::array<float, kValueCount> kDecibels = {-80.0f, -20.0f, 0.0f, 6.0f};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_NEAR(AmplitudeToDecibels(kAmplitudes[i]), kDecibels[i], kEpsilon);
    EXPECT_NEAR(DecibelsToAmplitude(kDecibels[i]), kAmplitudes[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_FLOAT_EQ(AmplitudeToDecibels(DecibelsToAmplitude(kDecibels[i])), kDecibels[i]);
    EXPECT_FLOAT_EQ(DecibelsToAmplitude(AmplitudeToDecibels(kAmplitudes[i])), kAmplitudes[i]);
  }
}

TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(AmplitudeToDecibels(0.0f), kMinDecibels);
  EXPECT_FLOAT_EQ(DecibelsToAmplitude(kMinDecibels), 0.0f);
}

}  // namespace
}  // namespace barely
