#include "barelymusician/dsp/sample_player.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Sampling rate.
constexpr Integer kSampleRate = 48000;

// Sample data.
constexpr Integer kDataLength = 5;
constexpr Real kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};

// Tests that the sample data is played back as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  for (Integer i = 0; i < kDataLength; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next(), kData[i]) << "at index " << i;
  }
  EXPECT_DOUBLE_EQ(sample_player.Next(), 0.0);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);
  sample_player.SetLoop(true);

  const Integer kLoopCount = 10;
  for (Integer i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next(), kData[i % kDataLength])
        << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);
  sample_player.SetLoop(true);

  const std::vector<Real> kSpeeds = {0.0, 0.4, 1.0, 1.25, 2.0, 3.3};
  for (const Real speed : kSpeeds) {
    sample_player.Reset();
    sample_player.SetSpeed(speed);

    for (Integer i = 0; i < kDataLength; ++i) {
      const Integer expected_index =
          static_cast<Integer>(static_cast<Real>(i) * speed);
      EXPECT_DOUBLE_EQ(sample_player.Next(),
                       kData[expected_index % kDataLength])
          << "at index " << i << ", where speed is: " << speed;
    }
  }
}

// Tests that the sample data is played back as expected at different sampling
// frequencies.
TEST(SamplePlayerTest, DifferentSampleFrequency) {
  const std::vector<Integer> kFrequencies = {0, kSampleRate / 3, kSampleRate,
                                             2 * kSampleRate, 5 * kSampleRate};
  SamplePlayer sample_player(kSampleRate);
  for (const Integer frequency : kFrequencies) {
    sample_player.Reset();
    sample_player.SetData(kData, frequency, kDataLength);
    sample_player.SetLoop(true);

    for (Integer i = 0; i < kDataLength; ++i) {
      const Integer expected_index = static_cast<Integer>(
          static_cast<Real>(i * frequency) / static_cast<Real>(kSampleRate));
      EXPECT_DOUBLE_EQ(sample_player.Next(),
                       kData[expected_index % kDataLength])
          << "at index " << i << ", where sample frequency is: " << frequency;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  const Real first_sample = sample_player.Next();
  EXPECT_NE(sample_player.Next(), first_sample);

  sample_player.Reset();
  EXPECT_DOUBLE_EQ(sample_player.Next(), first_sample);
}

}  // namespace
}  // namespace barely
