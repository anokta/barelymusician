#include "barelymusician/dsp/dsp_utils.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that converting values from/to amplitude and decibels returns expected
// results.
TEST(DspUtilsTest, AmplitudeDecibelsConversion) {
  const Real kEpsilon = 5e-2;

  const Integer kValueCount = 5;
  const Real kAmplitudes[kValueCount] = {0.0, 0.1, 0.25, 1.0, 2.0};
  const Real kDecibels[kValueCount] = {-80.0, -20.0, -12.0, 0.0, 6.0};

  for (Integer i = 0; i < kValueCount; ++i) {
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

// Tests that converting values from/to beats and nanoseconds returns expected
// results.
TEST(DspUtilsTest, BeatsNanosecondsConversion) {
  const Real kTempo = 120.0;
  const Integer kValueCount = 5;
  const Real kBeats[kValueCount] = {0.0, 1.0, 5.0, -4.0, -246.8};
  const Integer kNanoseconds[kValueCount] = {0, 500'000, 2'500'000, -2'000'000,
                                             -123'400'000};

  for (Integer i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(BeatsFromNanoseconds(kTempo, kNanoseconds[i]), kBeats[i]);
    EXPECT_EQ(NanosecondsFromBeats(kTempo, kBeats[i]), kNanoseconds[i]);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_DOUBLE_EQ(
        BeatsFromNanoseconds(kTempo, NanosecondsFromBeats(kTempo, kBeats[i])),
        kBeats[i]);
    EXPECT_EQ(NanosecondsFromBeats(
                  kTempo, BeatsFromNanoseconds(kTempo, kNanoseconds[i])),
              kNanoseconds[i]);
  }
}

// Tests that amplitude/decibels conversion snaps to `kMinDecibels` threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_DOUBLE_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0);
  EXPECT_DOUBLE_EQ(DecibelsFromAmplitude(0.0), kMinDecibels);
}

// Tests that converting values from/to frames and nanoseconds returns expected
// results.
TEST(DspUtilsTest, FramesNanosecondsConversion) {
  const Integer kFrameRate = 8000;
  const Integer kValueCount = 4;
  const Integer kFrames[kValueCount] = {0, 800, 4000, 32000};
  const Integer kNanoseconds[kValueCount] = {0, 100'000, 500'000, 4'000'000};

  for (Integer i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(FramesFromNanoseconds(kFrameRate, kNanoseconds[i]), kFrames[i]);
    EXPECT_EQ(NanosecondsFromFrames(kFrameRate, kFrames[i]), kNanoseconds[i]);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_EQ(FramesFromNanoseconds(
                  kFrameRate, NanosecondsFromFrames(kFrameRate, kFrames[i])),
              kFrames[i]);
    EXPECT_EQ(
        NanosecondsFromFrames(
            kFrameRate, FramesFromNanoseconds(kFrameRate, kNanoseconds[i])),
        kNanoseconds[i]);
  }
}

// Tests that the expected filter coefficients are generated for an arbitrary
// set of cutoff frequencies.
TEST(DspUtilsTest, GetFilterCoefficient) {
  const Real kEpsilon = 1e-2;
  const Integer kFrameRate = 8000;

  const Integer kCutoffCount = 5;
  const Real kCutoffs[kCutoffCount] = {0.0, 100.0, 500.0, 1000.0, 8000.0};
  const Real kExpectedCoefficients[kCutoffCount] = {1.00, 0.92, 0.68, 0.46,
                                                    0.00};

  for (Integer i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kFrameRate, kCutoffs[i]),
                kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting arbitrary pitches returns the expected frequencies.
TEST(DspUtilsTest, GetFrequency) {
  const Real kEpsilon = 1e-2;

  const Integer kPitcheCount = 5;
  const Real kPitches[kPitcheCount] = {-4.0, -0.75, 0.0, 2.0, 3.3};
  const Real kFrequencies[kPitcheCount] = {27.50, 261.62, 440.00, 1760.00,
                                           4333.63};

  for (Integer i = 0; i < kPitcheCount; ++i) {
    EXPECT_NEAR(GetFrequency(kPitches[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
