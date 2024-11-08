#include "dsp/sample_player.h"

#include <vector>

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Sampling rate.
constexpr int kSampleRate = 48000;

// Sample data.
constexpr int kDataLength = 5;
constexpr double kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};
constexpr bool kLoop = true;

// Tests that the sample data is played back as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next(/*loop=*/false), kData[i]) << "at index " << i;
  }
  EXPECT_DOUBLE_EQ(sample_player.Next(/*loop=*/false), 0.0);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  constexpr int kLoopCount = 10;
  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next(kLoop), kData[i % kDataLength]) << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  const std::vector<double> kSpeeds = {0.0, 0.4, 1.0, 1.25, 2.0, 3.3};
  for (const double speed : kSpeeds) {
    sample_player.Reset();
    sample_player.SetSpeed(speed);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index = static_cast<int>(static_cast<double>(i) * speed);
      EXPECT_DOUBLE_EQ(sample_player.Next(kLoop), kData[expected_index % kDataLength])
          << "at index " << i << ", where speed is: " << speed;
    }
  }
}

// Tests that the sample data is played back as expected at different sampling frequencies.
TEST(SamplePlayerTest, DifferentSampleFrequency) {
  const std::vector<int> kFrequencies = {0, kSampleRate / 3, kSampleRate, 2 * kSampleRate,
                                         5 * kSampleRate};
  SamplePlayer sample_player(kSampleRate);
  for (const int frequency : kFrequencies) {
    sample_player.Reset();
    sample_player.SetData(kData, frequency, kDataLength);

    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<double>(i * frequency) / static_cast<double>(kSampleRate));
      EXPECT_DOUBLE_EQ(sample_player.Next(kLoop), kData[expected_index % kDataLength])
          << "at index " << i << ", where sample frequency is: " << frequency;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player(kSampleRate);
  sample_player.SetData(kData, kSampleRate, kDataLength);

  const double first_sample = sample_player.Next(/*loop=*/false);
  EXPECT_NE(sample_player.Next(/*loop=*/false), first_sample);

  sample_player.Reset();
  EXPECT_DOUBLE_EQ(sample_player.Next(/*loop=*/false), first_sample);
}

}  // namespace
}  // namespace barely::internal
