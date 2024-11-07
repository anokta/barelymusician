#include "dsp/envelope.h"

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Sampling rate.
constexpr int kSampleRate = 1000;

// Envelope ADSR.
constexpr double kAttack = 0.02;
constexpr double kDecay = 1.0;
constexpr double kSustain = 0.5;
constexpr double kRelease = 0.8;

// Tolerated error margin.
constexpr double kEpsilon = 1e-3;

// Tests that the envelope generates the expected output samples when initialized with the default
// constructor.
TEST(EnvelopeTest, ProcessDefault) {
  Envelope envelope(kSampleRate);
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);

  envelope.Start();
  EXPECT_DOUBLE_EQ(envelope.Next(), 1.0);

  envelope.Stop();
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);
}

// Tests that the envelope generates the expected output samples consistently over multiple samples.
TEST(EnvelopeTest, ProcessMultiSamples) {
  constexpr int kAttackSampleCount = static_cast<int>(kSampleRate * kAttack);
  constexpr int kDecaySampleCount = static_cast<int>(kSampleRate * kDecay);
  constexpr int kSustainSampleCount = kAttackSampleCount + kDecaySampleCount;
  constexpr int kReleaseSampleCount = static_cast<int>(kSampleRate * kRelease);

  Envelope envelope(kSampleRate);
  envelope.SetAttack(kAttack);
  envelope.SetDecay(kDecay);
  envelope.SetSustain(kSustain);
  envelope.SetRelease(kRelease);
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);

  double expected_sample = 0.0;

  envelope.Start();
  for (int i = 0; i < kSustainSampleCount + kSampleRate; ++i) {
    if (i < kAttackSampleCount) {
      // Attack.
      expected_sample = static_cast<double>(i) / static_cast<double>(kAttackSampleCount);
    } else if (i < kSustainSampleCount) {
      // Decay.
      expected_sample = 1.0 - kSustain * static_cast<double>(i - kAttackSampleCount) /
                                  static_cast<double>(kDecaySampleCount);
    } else {
      // Sustain.
      expected_sample = kSustain;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }

  envelope.Stop();
  for (int i = 0; i < kReleaseSampleCount + kSampleRate; ++i) {
    if (i < kReleaseSampleCount) {
      // Release.
      expected_sample =
          (1.0 - static_cast<double>(i) / static_cast<double>(kReleaseSampleCount)) * kSustain;
    } else {
      // Idle.
      expected_sample = 0.0;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }
}

}  // namespace
}  // namespace barely::internal
