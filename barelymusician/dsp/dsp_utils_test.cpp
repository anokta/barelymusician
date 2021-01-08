#include "barelymusician/dsp/dsp_utils.h"

#include <cstdint>

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
    EXPECT_NEAR(AmplitudeFromDecibels(kDecibels[i]), kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(kAmplitudes[i]), kDecibels[i], kEpsilon);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_NEAR(AmplitudeFromDecibels(DecibelsFromAmplitude(kAmplitudes[i])),
                kAmplitudes[i], kEpsilon);
    EXPECT_NEAR(DecibelsFromAmplitude(AmplitudeFromDecibels(kDecibels[i])),
                kDecibels[i], kEpsilon);
  }
}

// Tests that amplitude/decibels conversion snaps to |kMinDecibels| threshold.
TEST(DspUtilsTest, AmplitudeDecibelsMinThreshold) {
  EXPECT_FLOAT_EQ(AmplitudeFromDecibels(kMinDecibels), 0.0f);
  EXPECT_FLOAT_EQ(DecibelsFromAmplitude(0.0f), kMinDecibels);
}

// Tests that converting arbitrary note indices returns expected frequencies.
TEST(DspUtilsTest, FrequencyFromPitch) {
  const int kNumIndices = 4;
  const float kIndices[kNumIndices] = {21.0f, 60.0f, 69.0f, 90.5f};
  const float kFrequencies[kNumIndices] = {27.5f, 261.6f, 440.0f, 1523.3f};

  for (int i = 0; i < kNumIndices; ++i) {
    EXPECT_NEAR(FrequencyFromPitch(kIndices[i]), kFrequencies[i], kEpsilon);
  }
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
    EXPECT_NEAR(GetFilterCoefficient(kSampleRate, kCutoffs[i]),
                kExpectedCoefficients[i], kEpsilon);
  }
}

// Tests that converting values from/to samples and seconds returns expected
// results.
TEST(DspUtilsTest, SamplesSecondsConversion) {
  const int kSampleRate = 8000;
  const int kNumValues = 4;
  const std::int64_t kSamples[kNumValues] = {0, 800, 4000, 32000};
  const double kSeconds[kNumValues] = {0.0, 0.1, 0.5, 4.0};

  for (int i = 0; i < kNumValues; ++i) {
    EXPECT_EQ(SamplesFromSeconds(kSampleRate, kSeconds[i]), kSamples[i]);
    EXPECT_DOUBLE_EQ(SecondsFromSamples(kSampleRate, kSamples[i]), kSeconds[i]);

    // Verify that the back and forth conversion do not mutate the value.
    EXPECT_EQ(SamplesFromSeconds(kSampleRate,
                                 SecondsFromSamples(kSampleRate, kSamples[i])),
              kSamples[i]);
    EXPECT_DOUBLE_EQ(
        SecondsFromSamples(kSampleRate,
                           SamplesFromSeconds(kSampleRate, kSeconds[i])),
        kSeconds[i]);
  }
}

}  // namespace
}  // namespace barelyapi
