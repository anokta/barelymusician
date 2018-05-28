#include "barelymusician/dsp/sample_player.h"

#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
constexpr int kSampleRate = 48000;
constexpr float kSampleInterval = 1.0f / static_cast<float>(kSampleRate);

// Sample data.
constexpr int kDataLength = 5;
constexpr float kData[kDataLength] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

// Tests that the sample data is played back as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player(kSampleInterval);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_EQ(kData[i], sample_player.Next()) << "at index " << i;
  }
  EXPECT_EQ(0.0f, sample_player.Next());
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  SamplePlayer sample_player(kSampleInterval);
  sample_player.SetData(kData, kSampleRate, kDataLength);
  sample_player.SetLoop(true);

  const int kNumLoops = 10;
  for (int i = 0; i < kDataLength * kNumLoops; ++i) {
    EXPECT_EQ(kData[i % kDataLength], sample_player.Next()) << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  SamplePlayer sample_player(kSampleInterval);
  sample_player.SetData(kData, kSampleRate, kDataLength);
  sample_player.SetLoop(true);

  const std::vector<float> kSpeeds = {0.0f, 0.4f, 1.0f, 1.25f, 2.0f, 3.0f};
  for (const float speed : kSpeeds) {
    sample_player.Reset();
    sample_player.SetSpeed(speed);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<float>(i) * speed);
      EXPECT_EQ(kData[expected_index % kDataLength], sample_player.Next())
          << "at index " << i << ", where speed is: " << speed;
    }
  }
}

// Tests that the sample data is played back as expected at different sampling
// frequencies.
TEST(SamplePlayerTest, DifferentSampleFrequency) {
  const std::vector<int> kFrequencies = {0, kSampleRate / 3, kSampleRate,
                                         2 * kSampleRate, 5 * kSampleRate};  
  SamplePlayer sample_player(kSampleInterval);
  for (const int frequency : kFrequencies) {
    sample_player.Reset();
    sample_player.SetData(kData, frequency, kDataLength);
    sample_player.SetLoop(true);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<float>(i * frequency) * kSampleInterval);
      EXPECT_EQ(kData[expected_index % kDataLength], sample_player.Next())
          << "at index " << i << ", where sample frequency is: " << frequency;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player(kSampleInterval);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  const float first_sample = sample_player.Next();
  EXPECT_NE(first_sample, sample_player.Next());

  sample_player.Reset();
  EXPECT_EQ(first_sample, sample_player.Next());
}

}  // namespace
}  // namespace barelyapi
