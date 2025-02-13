#include "dsp/sample_player.h"

#include <cmath>
#include <vector>

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Sample data.
constexpr int kDataLength = 5;
constexpr float kData[kDataLength] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
constexpr float kPitch = 0.0f;
constexpr float kSampleInterval = 1.0f;
constexpr SampleDataSlice kSlice = SampleDataSlice(kPitch, 1, kData);

// Tests that the sample data is played back once as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kPitch, kSampleInterval);

  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_FLOAT_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), kData[i]) << "at index " << i;
  }
  EXPECT_FLOAT_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), 0.0f);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  constexpr int kLoopCount = 10;

  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);
  sample_player.SetIncrement(kPitch, kSampleInterval);

  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_FLOAT_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(), kData[i % kDataLength])
        << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  const std::vector<float> kPitches = {-1.0f, 0.0f, 0.4f, 1.0f, 1.25f, 2.0f, 3.3f};

  SamplePlayer sample_player;
  sample_player.SetSlice(&kSlice);

  for (const float pitch : kPitches) {
    sample_player.Reset();
    sample_player.SetIncrement(pitch, kSampleInterval);
    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index =
          static_cast<int>(static_cast<float>(i) * std::pow(2.0f, pitch - kPitch));
      EXPECT_FLOAT_EQ(sample_player.Next<SamplePlaybackMode::kLoop>(),
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

  const float first_sample = sample_player.Next<SamplePlaybackMode::kOnce>();
  EXPECT_NE(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);

  sample_player.Reset();
  EXPECT_FLOAT_EQ(sample_player.Next<SamplePlaybackMode::kOnce>(), first_sample);
}

}  // namespace
}  // namespace barely
