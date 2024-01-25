#include "barelymusician/dsp/envelope.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

// Frame rate.
constexpr int kFrameRate = 1000;

// Envelope ADSR.
constexpr float kAttack = 0.02f;
constexpr float kDecay = 1.0f;
constexpr float kSustain = 0.5f;
constexpr float kRelease = 0.8f;

// Tolerated error margin.
constexpr float kEpsilon = 1e-3f;

// Tests that the envelope generates the expected output samples when initialized with the default
// constructor.
TEST(EnvelopeTest, ProcessDefault) {
  Envelope envelope(kFrameRate);
  EXPECT_EQ(envelope.Next(), 0.0f);

  envelope.Start();
  EXPECT_EQ(envelope.Next(), 1.0f);

  envelope.Stop();
  EXPECT_EQ(envelope.Next(), 0.0f);
}

// Tests that the envelope generates the expected output samples consistently over multiple samples.
TEST(EnvelopeTest, ProcessMultiSamples) {
  constexpr int kAttackSampleCount = static_cast<int>(kFrameRate * kAttack);
  constexpr int kDecaySampleCount = static_cast<int>(kFrameRate * kDecay);
  constexpr int kSustainSampleCount = kAttackSampleCount + kDecaySampleCount;
  constexpr int kReleaseSampleCount = static_cast<int>(kFrameRate * kRelease);

  Envelope envelope(kFrameRate);
  envelope.SetAttack(kAttack);
  envelope.SetDecay(kDecay);
  envelope.SetSustain(kSustain);
  envelope.SetRelease(kRelease);
  EXPECT_EQ(envelope.Next(), 0.0f);

  float expected_sample = 0.0f;

  envelope.Start();
  for (int i = 0; i < kSustainSampleCount + kFrameRate; ++i) {
    if (i < kAttackSampleCount) {
      // Attack.
      expected_sample = static_cast<float>(i) / static_cast<float>(kAttackSampleCount);
    } else if (i < kSustainSampleCount) {
      // Decay.
      expected_sample = 1.0f - kSustain * static_cast<float>(i - kAttackSampleCount) /
                                   static_cast<float>(kDecaySampleCount);
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
      expected_sample =
          (1.0f - static_cast<float>(i) / static_cast<float>(kReleaseSampleCount)) * kSustain;
    } else {
      // Idle.
      expected_sample = 0.0f;
    }
    EXPECT_NEAR(envelope.Next(), expected_sample, kEpsilon);
  }
}

}  // namespace
}  // namespace barely
