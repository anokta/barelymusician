#include "dsp/sample_player.h"

#include <vector>

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Sample data.
constexpr int kDataLength = 5;
constexpr double kData[kDataLength] = {1.0, 2.0, 3.0, 4.0, 5.0};
constexpr SampleDataSlice kSlice = SampleDataSlice(0.0, 48000, kData);

// Tests that the sample data is played back once as expected.
TEST(SamplePlayerTest, SimplePlayback) {
  constexpr double kIncrement = 1.0;

  double cursor = 0.0;
  for (int i = 0; i < kDataLength; ++i) {
    EXPECT_DOUBLE_EQ(PlaySample<SamplePlaybackMode::kOnce>(&kSlice, kIncrement, cursor), kData[i])
        << "at index " << i;
  }
  EXPECT_DOUBLE_EQ(PlaySample<SamplePlaybackMode::kOnce>(&kSlice, kIncrement, cursor), 0.0);
}

// Tests that the sample data is played back as expected, when set to loop.
TEST(SamplePlayerTest, SimplePlaybackLoop) {
  constexpr double kIncrement = 1.0;
  constexpr int kLoopCount = 10;

  double cursor = 0.0;
  for (int i = 0; i < kDataLength * kLoopCount; ++i) {
    EXPECT_DOUBLE_EQ(PlaySample<SamplePlaybackMode::kLoop>(&kSlice, kIncrement, cursor),
                     kData[i % kDataLength])
        << "at index " << i;
  }
}

// Tests that the sample data is played back as expected at different speeds.
TEST(SamplePlayerTest, SetSpeed) {
  const std::vector<double> kIncrements = {0.0, 0.4, 1.0, 1.25, 2.0, 3.3};

  for (const double increment : kIncrements) {
    double cursor = 0.0;
    for (int i = 0; i < kDataLength; ++i) {
      const int expected_index = static_cast<int>(static_cast<double>(i) * increment);
      EXPECT_DOUBLE_EQ(PlaySample<SamplePlaybackMode::kLoop>(&kSlice, increment, cursor),
                       kData[expected_index % kDataLength])
          << "at index " << i << ", where increment is: " << increment;
    }
  }
}

}  // namespace
}  // namespace barely::internal
