#include "barelymusician/dsp/dsp_utils.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that converting values from/to amplitude and decibels returns expected
// results.
TEST(DspUtilsTest, AmplitudeDecibelsConversion) {
  const double kEpsilon = 5e-2;

  const int kNumValues = 5;
  const double kAmplitudes[kNumValues] = {0.0, 0.1, 0.25, 1.0, 2.0};
  const double kDecibels[kNumValues] = {-80.0, -20.0, -12.0, 0.0, 6.0};

  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_NEAR(AmplitudeFromDecibels(kDecibels[i]), kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(kAmplitudes[i]), kDecibels[i], kEpsilon);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_DOUBLE_EQ(
        AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])),
        kAmplitudes[i]);
    EXPECT_DOUBLE_EQ(DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])),
                     kDecibels[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_DOUBLE_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0);
  EXPECT_DOUBLE_EQ(DecibelsFromAmplitude(0.0), kMinDecibels);
}

// Tests that the expected filter coefficients are generated for an arbitrary
// set of cutoff frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  const double kEpsilon = 1e-2;
  const int kSampleRate = 8000;

  const int kNumCutoffs = 5;
  const double kCutoffs[kNumCutoffs] = {0.0, 100.0, 500.0, 1000.0, 8000.0};
  const double kExpectedCoefficients[kNumCutoffs] = {1.00, 0.92, 0.68, 0.46,
                                                     0.00};

  for (int i = 0; i < kNumCutoffs; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kSampleRate, kCutoffs[i]),
                kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting arbitrary pitches returns the expected frequencies.
TEST(DspUtilsTest, GetFrequency) {
  const double kEpsilon = 1e-2;

  const int kNumPitches = 5;
  const double kPitches[kNumPitches] = {-4.0, -0.75, 0.0, 2.0, 3.3};
  const double kFrequencies[kNumPitches] = {27.50, 261.62, 440.00, 1760.00,
                                            4333.63};

  for (int i = 0; i < kNumPitches; ++i) {
    EXPECT_NEAR(GetFrequency(kPitches[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barelyapi
