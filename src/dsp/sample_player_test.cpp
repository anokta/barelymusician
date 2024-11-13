#include "dsp/sample_player.h"

#include <vector>

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Sample data.
constexpr int kDataLength = 5;
constexpr double kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};
constexpr double kPitch = 0.0;
constexpr double kSampleInterval = 1;
constexpr SampleDataSlice kSlice = SampleDataSlice(kPitch, 1, kData);

// Tests that the sample data is played back once as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kPitch, kSampleInterval);

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
  sample_player.SetIncrement(kPitch, kSampleInterval);

  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(), kData[i % kDataLength])
        << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  const std::vector<double> kPitches = {-1.0, 0.0, 0.4, 1.0, 1.25, 2.0, 3.3};

  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);

  for (const double pitch : kPitches) {
    sample_player.Reset();
    sample_player.SetIncrement(pitch, kSampleInterval);
    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<double>(i) * std::pow(2.0, pitch - kPitch));
      EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(),
                       kData[expected_index % kDataLength])
          << "at index " << i << ", where pitch is: " << pitch;
    }
  }
}

// Tests that the sample player resets its state correctly.
TEST(SamplePlayerTest, Reset) {
  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kPitch, kSampleInterval);

  const double first_sample = sample_player.Next<SamplePlaybackMode::kOnce>();
  EXPECT_NE(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);

  sample_player.Reset();
  EXPECT_DOUBLE_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);
}

}  // namespace
}  // namespace barely::internal
