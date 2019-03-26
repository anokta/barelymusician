#include "barelymusician/dsp/envelope.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 1000;
const float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Envelope ADSR.
const float kAttack = 0.02f;
const float kDecay = 1.0f;
const float kSustain = 0.5f;
const float kRelease = 0.8f;

// Tolerated error margin.
const float kEpsilon = 1e-3f;

// Tests that the envelope generates the expected output samples when
// initialized with the default constructor.
TEST(EnvelopeTest, ProcessDefault) {
  Envelope envelope(kSampleInterval);
  EXPECT_FLOAT_EQ(0.0f, envelope.Next());

  envelope.Start();
  EXPECT_FLOAT_EQ(1.0f, envelope.Next());

  envelope.Stop();
  EXPECT_FLOAT_EQ(0.0f, envelope.Next());
}

// Tests that the envelope generates the expected output samples consistently
// over multiple samples.
TEST(EnvelopeTest, ProcessMultiSamples) {
  const int kNumAttackSamples = static_cast<int>(kSampleRate * kAttack);
  const int kNumDecaySamples = static_cast<int>(kSampleRate * kDecay);
  const int kNumSamplesForSustain = kNumAttackSamples + kNumDecaySamples;
  const int kNumReleaseSamples = static_cast<int>(kSampleRate * kRelease);

  Envelope envelope(kSampleInterval);
  envelope.SetAttack(kAttack);
  envelope.SetDecay(kDecay);
  envelope.SetSustain(kSustain);
  envelope.SetRelease(kRelease);
  EXPECT_FLOAT_EQ(0.0f, envelope.Next());

  float expected_sample = 0.0f;

  envelope.Start();
  for (int i = 0; i < kNumSamplesForSustain + kSampleRate; ++i) {
    if (i < kNumAttackSamples) {
      // Attack.
      expected_sample =
          static_cast<float>(i) / static_cast<float>(kNumAttackSamples);
    } else if (i < kNumSamplesForSustain) {
      // Decay.
      expected_sample = 1.0f - kSustain *
                                   static_cast<float>(i - kNumAttackSamples) /
                                   static_cast<float>(kNumDecaySamples);
    } else {
      // Sustain.
      expected_sample = kSustain;
    }
    EXPECT_NEAR(expected_sample, envelope.Next(), kEpsilon);
  }

  envelope.Stop();
  for (int i = 0; i < kNumReleaseSamples + kSampleRate; ++i) {
    if (i < kNumReleaseSamples) {
      // Release.
      expected_sample = (1.0f - static_cast<float>(i) /
                                    static_cast<float>(kNumReleaseSamples)) *
                        kSustain;
    } else {
      // Idle.
      expected_sample = 0.0f;
    }
    EXPECT_NEAR(expected_sample, envelope.Next(), kEpsilon);
  }
}

}  // namespace
}  // namespace barelyapi
