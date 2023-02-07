#include "barelymusician/dsp/envelope.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

// Frame rate.
constexpr int kFrameRate = 1000;

// Envelope ADSR.
constexpr double kAttack = 0.02;
constexpr double kDecay = 1.0;
constexpr double kSustain = 0.5;
constexpr double kRelease = 0.8;

// Tolerated error margin.
constexpr double kEpsilon = 1e-3;

// Tests that the envelope generates the expected output samples when
// initialized with the default constructor.
TEST(EnvelopeTest, ProcessDefault) {
  Envelope envelope(kFrameRate);
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);

  envelope.Start();
  EXPECT_DOUBLE_EQ(envelope.Next(), 1.0);

  envelope.Stop();
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);
}

// Tests that the envelope generates the expected output samples consistently
// over multiple samples.
TEST(EnvelopeTest, ProcessMultiSamples) {
  const int kAttackSampleCount = static_cast<int>(kFrameRate * kAttack);
  const int kDecaySampleCount = static_cast<int>(kFrameRate * kDecay);
  const int kSustainSampleCount = kAttackSampleCount + kDecaySampleCount;
  const int kReleaseSampleCount = static_cast<int>(kFrameRate * kRelease);

  Envelope envelope(kFrameRate);
  envelope.SetAttack(kAttack);
  envelope.SetDecay(kDecay);
  envelope.SetSustain(kSustain);
  envelope.SetRelease(kRelease);
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);

  double expected_sample = 0.0;

  envelope.Start();
  for (int i = 0; i < kSustainSampleCount + kFrameRate; ++i) {
    if (i < kAttackSampleCount) {
      // Attack.
      expected_sample =
          static_cast<double>(i) / static_cast<double>(kAttackSampleCount);
    } else if (i < kSustainSampleCount) {
      // Decay.
      expected_sample = 1.0 - kSustain *
                                  static_cast<double>(i - kAttackSampleCount) /
                                  static_cast<double>(kDecaySampleCount);
    } else {
      // Sustain.
      expected_sample = kSustain;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }

  envelope.Stop();
  for (int i = 0; i < kReleaseSampleCount + kFrameRate; ++i) {
    if (i < kReleaseSampleCount) {
      // Release.
      expected_sample = (1.0 - static_cast<double>(i) /
                                   static_cast<double>(kReleaseSampleCount)) *
                        kSustain;
    } else {
      // Idle.
      expected_sample = 0.0;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }
}

}  // namespace
}  // namespace barely
