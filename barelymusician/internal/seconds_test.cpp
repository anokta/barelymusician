#include "barelymusician/internal/seconds.h"

#include <array>
#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that arbitrary values are converted between beats and seconds as expected.
TEST(SecondsTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(BeatsFromSeconds(kTempo, kSeconds[i]), kBeats[i]);
    EXPECT_DOUBLE_EQ(SecondsFromBeats(kTempo, kBeats[i]), kSeconds[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(BeatsFromSeconds(kTempo, SecondsFromBeats(kTempo, kBeats[i])), kBeats[i]);
    EXPECT_DOUBLE_EQ(SecondsFromBeats(kTempo, BeatsFromSeconds(kTempo, kSeconds[i])), kSeconds[i]);
  }
}

// Tests that arbitrary values are converted between frames and seconds as expected.
TEST(SecondsTest, FramesSecondsConversion) {
  constexpr int kFrameRate = 8000;

  constexpr int kValueCount = 4;
  constexpr std::array<int64_t, kValueCount> kFrames = {0, 800, 4000, 32000};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.1, 0.5, 4.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(FramesFromSeconds(kFrameRate, kSeconds[i]), kFrames[i]);
    EXPECT_DOUBLE_EQ(SecondsFromFrames(kFrameRate, kFrames[i]), kSeconds[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(FramesFromSeconds(kFrameRate, SecondsFromFrames(kFrameRate, kFrames[i])), kFrames[i]);
    EXPECT_DOUBLE_EQ(SecondsFromFrames(kFrameRate, FramesFromSeconds(kFrameRate, kSeconds[i])),
                     kSeconds[i]);
  }
}

}  // namespace
}  // namespace barely
