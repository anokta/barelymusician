#include "barelymusician/dsp/dsp_utils.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that converting values from/to amplitude and decibels returns expected
// results.
TEST(DspUtilsTest, AmplitudeDecibelsConversion) {
  const float kEpsilon = 5e-2f;

  const int kNumValues = 5;
  const float kAmplitudes[kNumValues] = {0.0f, 0.1f, 0.25f, 1.0f, 2.0f};
  const float kDecibels[kNumValues] = {-80.0f, -20.0f, -12.0f, 0.0f, 6.0f};

  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_NEAR(AmplitudeFromDecibels(kDecibels[i]), kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(kAmplitudes[i]), kDecibels[i], kEpsilon);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_FLOAT_EQ(
        AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])),
        kAmplitudes[i]);
    EXPECT_FLOAT_EQ(DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])),
                    kDecibels[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0f);
  EXPECT_FLOAT_EQ(DecibelsFromAmplitude(0.0f), kMinDecibels);
}

// Tests that the expected filter coefficients are generated for an arbitrary
// set of cutoff frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  const float kEpsilon = 1e-2f;
  const int kSampleRate = 8000;

  const int kNumCutoffs = 5;
  const float kCutoffs[kNumCutoffs] = {0.0f, 100.0f, 500.0f, 1000.0f, 8000.0f};
  const float kExpectedCoefficients[kNumCutoffs] = {1.00f, 0.92f, 0.68f, 0.46f,
                                                    0.00f};

  for (int i = 0; i < kNumCutoffs; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kSampleRate, kCutoffs[i]),
                kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting arbitrary pitches returns the expected frequencies.
TEST(DspUtilsTest, GetFrequency) {
  const float kEpsilon = 1e-2f;

  const int kNumPitches = 5;
  const float kPitches[kNumPitches] = {-4.0f, -0.75f, 0.0f, 2.0f, 3.3f};
  const float kFrequencies[kNumPitches] = {27.50f, 261.62f, 440.00f, 1760.00f,
                                           4333.63f};

  for (int i = 0; i < kNumPitches; ++i) {
    EXPECT_NEAR(GetFrequency(kPitches[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barelyapi
