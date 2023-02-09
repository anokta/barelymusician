#include "barelymusician/common/seconds.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that beats/seconds conversion returns expected results.
TEST(SecondsTest, BeatsSecondsConversion) {
  const double kTempo = 120.0;

  const int kValueCount = 5;
  const std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  const std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

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

// Tests that frames/seconds conversion returns expected results.
TEST(SecondsTest, FramesSecondsConversion) {
  const int kFrameRate = 8000;

  const int kValueCount = 4;
  const std::array<int, kValueCount> kFrames = {0, 800, 4000, 32000};
  const std::array<double, kValueCount> kSeconds = {0.0, 0.1, 0.5, 4.0};

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
