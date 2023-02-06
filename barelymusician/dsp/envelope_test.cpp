#include "barelymusician/dsp/envelope.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Sampling rate.
constexpr Integer kFrameRate = 1000;

// Envelope ADSR.
constexpr Real kAttack = 0.02;
constexpr Real kDecay = 1.0;
constexpr Real kSustain = 0.5;
constexpr Real kRelease = 0.8;

// Tolerated error margin.
constexpr Real kEpsilon = 1e-3;

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
  const Integer kAttackSampleCount = static_cast<Integer>(kFrameRate * kAttack);
  const Integer kDecaySampleCount = static_cast<Integer>(kFrameRate * kDecay);
  const Integer kSustainSampleCount = kAttackSampleCount + kDecaySampleCount;
  const Integer kReleaseSampleCount =
      static_cast<Integer>(kFrameRate * kRelease);

  Envelope envelope(kFrameRate);
  envelope.SetAttack(kAttack);
  envelope.SetDecay(kDecay);
  envelope.SetSustain(kSustain);
  envelope.SetRelease(kRelease);
  EXPECT_DOUBLE_EQ(envelope.Next(), 0.0);

  Real expected_sample = 0.0;

  envelope.Start();
  for (Integer i = 0; i < kSustainSampleCount + kFrameRate; ++i) {
    if (i < kAttackSampleCount) {
      // Attack.
      expected_sample =
          static_cast<Real>(i) / static_cast<Real>(kAttackSampleCount);
    } else if (i < kSustainSampleCount) {
      // Decay.
      expected_sample = 1.0 - kSustain *
                                  static_cast<Real>(i - kAttackSampleCount) /
                                  static_cast<Real>(kDecaySampleCount);
    } else {
      // Sustain.
      expected_sample = kSustain;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }

  envelope.Stop();
  for (Integer i = 0; i < kReleaseSampleCount + kFrameRate; ++i) {
    if (i < kReleaseSampleCount) {
      // Release.
      expected_sample = (1.0 - static_cast<Real>(i) /
                                   static_cast<Real>(kReleaseSampleCount)) *
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
