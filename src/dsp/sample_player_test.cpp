#include "dsp/sample_player.h"

#include <vector>

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Sample data.
constexpr int kDataLength = 5;
constexpr double kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};
constexpr SampleDataSlice kSlice = SampleDataSlice(0.0, 1, kData);
constexpr double kSampleInterval = 1;
constexpr double kSpeed = 1.0;

// Tests that the sample data is played back once as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kSpeed, kSampleInterval);

  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), kData[i]) << "at index " << i;
  }
  EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), 0.0);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  constexpr int kLoopCount = 10;

  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kSpeed, kSampleInterval);

  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(), kData[i % kDataLength])
        << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  const std::vector<double> kSpeeds = {0.0, 0.4, 1.0, 1.25, 2.0, 3.3};

  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);

  for (const double speed : kSpeeds) {
    sample_player.Reset();
    sample_player.SetIncrement(speed, kSampleInterval);
    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index = static_cast<int>(static_cast<double>(i) * speed);
      EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(),
                       kData[expected_index % kDataLength])
          << "at index " << i << ", where speed is: " << speed;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kSpeed, kSampleInterval);

  const double first_sample = sample_player.Next<SamplePlaybackMode::kOnce>();
  EXPECT_NE(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);

  sample_player.Reset();
  EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);
}

}  // namespace
}  // namespace barely::internal
