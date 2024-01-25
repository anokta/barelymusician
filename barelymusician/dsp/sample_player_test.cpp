#include "barelymusician/dsp/sample_player.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Frame rate.
constexpr int kFrameRate = 48000;

// Sample data.
constexpr int kDataLength = 5;
constexpr float kData[kDataLength] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

// Tests that the sample data is played back as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player(kFrameRate);
  sample_player.SetData(kData, kFrameRate, kDataLength);

  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_EQ(sample_player.Next(), kData[i]) << "at index " << i;
  }
  EXPECT_EQ(sample_player.Next(), 0.0f);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  SamplePlayer sample_player(kFrameRate);
  sample_player.SetData(kData, kFrameRate, kDataLength);
  sample_player.SetLoop(true);

  constexpr int kLoopCount = 10;
  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_EQ(sample_player.Next(), kData[i % kDataLength]) << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  SamplePlayer sample_player(kFrameRate);
  sample_player.SetData(kData, kFrameRate, kDataLength);
  sample_player.SetLoop(true);

  const std::vector<float> kSpeeds = {0.0f, 0.4f, 1.0f, 1.25f, 2.0f, 3.3f};
  for (const float speed : kSpeeds) {
    sample_player.Reset();
    sample_player.SetSpeed(speed);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index = static_cast<int>(static_cast<float>(i) * speed);
      EXPECT_EQ(sample_player.Next(), kData[expected_index % kDataLength])
          << "at index " << i << ", where speed is: " << speed;
    }
  }
}

// Tests that the sample data is played back as expected at different sampling frequencies.
TEST(SamplePlayerTest, DifferentSampleFrequency) {
  const std::vector<int> kFrequencies = {0, kFrameRate / 3, kFrameRate, 2 * kFrameRate,
                                         5 * kFrameRate};
  SamplePlayer sample_player(kFrameRate);
  for (const int frequency : kFrequencies) {
    sample_player.Reset();
    sample_player.SetData(kData, frequency, kDataLength);
    sample_player.SetLoop(true);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<float>(i * frequency) / static_cast<float>(kFrameRate));
      EXPECT_EQ(sample_player.Next(), kData[expected_index % kDataLength])
          << "at index " << i << ", where sample frequency is: " << frequency;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player(kFrameRate);
  sample_player.SetData(kData, kFrameRate, kDataLength);

  const float first_sample = sample_player.Next();
  EXPECT_NE(sample_player.Next(), first_sample);

  sample_player.Reset();
  EXPECT_EQ(sample_player.Next(), first_sample);
}

}  // namespace
}  // namespace barely
