#include "barelymusician/common/seconds.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the beats/seconds conversion returns expected results.
TEST(SecondsTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0,
                                                      -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0,
                                                        -12.3};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(BeatsFromSeconds(kTempo, kSeconds[i]), kBeats[i]);
    EXPECT_DOUBLE_EQ(SecondsFromBeats(kTempo, kBeats[i]), kSeconds[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(
        BeatsFromSeconds(kTempo, SecondsFromBeats(kTempo, kBeats[i])),
        kBeats[i]);
    EXPECT_DOUBLE_EQ(
        SecondsFromBeats(kTempo, BeatsFromSeconds(kTempo, kSeconds[i])),
        kSeconds[i]);
  }
}

// Tests that the frames/seconds conversion returns expected results.
TEST(SecondsTest, FramesSecondsConversion) {
  constexpr int kFrameRate = 8000;

  constexpr int kValueCount = 4;
  constexpr std::array<int, kValueCount> kFrames = {0, 800, 4000, 32000};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.1, 0.5, 4.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(FramesFromSeconds(kFrameRate, kSeconds[i]), kFrames[i]);
    EXPECT_DOUBLE_EQ(SecondsFromFrames(kFrameRate, kFrames[i]), kSeconds[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(FramesFromSeconds(kFrameRate,
                                SecondsFromFrames(kFrameRate, kFrames[i])),
              kFrames[i]);
    EXPECT_DOUBLE_EQ(
        SecondsFromFrames(kFrameRate,
                          FramesFromSeconds(kFrameRate, kSeconds[i])),
        kSeconds[i]);
  }
}

}  // namespace
}  // namespace barely
