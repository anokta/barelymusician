#include "barelymusician/dsp/dsp_utils.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tolerated error margin.
constexpr float kEpsilon = 5e-2f;

// Tests that converting values from/to amplitude and decibels returns expected
// results.
TEST(DspUtilsTest, AmplitudeDecibelsConversion) {
  const int kNumValues = 5;
  const float kAmplitudes[kNumValues] = {0.0f, 0.1f, 0.25f, 1.0f, 2.0f};
  const float kDecibels[kNumValues] = {-80.0f, -20.0f, -12.0f, 0.0f, 6.0f};

  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_NEAR(kAmplitudes[i], AmplitudeFromDecibels(kDecibels[i]), kEpsilon);
    EXPECT_NEAR(kDecibels[i], DecibelsFromAmplitude(kAmplitudes[i]), kEpsilon);

    // Verify that the conversion methods do not mutate the value.
    EXPECT_NEAR(kAmplitudes[i],
                AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])),
                kEpsilon);
    EXPECT_NEAR(kDecibels[i],
                DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])),
                kEpsilon);
  }
}

// Tests that amplitude/decibels conversion snaps to |kMinDecibels| threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(0.0f, AmplitudeFromDecibels(kMinDecibels));
  EXPECT_FLOAT_EQ(kMinDecibels, DecibelsFromAmplitude(0.0f));
}

// Tests that the expected filter coefficients are generated for an arbitrary
// set of cutoff frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  const int kSampleRate = 8000;

  const int kNumCutoffs = 5;
  const float kCutoffs[kNumCutoffs] = {0.0f, 100.0f, 500.0f, 1000.0f, 8000.0f};
  const float kExpectedCoefficients[kNumCutoffs] = {1.0f, 0.92f, 0.68f, 0.46f,
                                                    0.0f};

  for (int i = 0; i < kNumCutoffs; ++i) {
    EXPECT_NEAR(kExpectedCoefficients[i],
                GetFilterCoefficient(kSampleRate, kCutoffs[i]), kEpsilon);
  }
}

}  // namespace
}  // namespace barelyapi
