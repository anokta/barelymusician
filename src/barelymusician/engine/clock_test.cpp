#include "barelymusician/engine/clock.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
constexpr int kSampleRate = 48000;

// Clock tempo.
constexpr double kTempo = 120.0;

// Tests that the clock sets its tempo as expected.
TEST(ClockTest, SetTempo) {
  Clock clock(kSampleRate);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);

  clock.SetTempo(kTempo);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), kTempo);
}

// Tests that the clock sets its current position as expected.
TEST(ClockTest, SetPosition) {
  const double kPosition = 2.75f;

  Clock clock(kSampleRate);
  EXPECT_EQ(clock.GetPosition(), 0.0);

  clock.SetPosition(kPosition);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), kPosition);
}

// Tests that the clock updates its position as expected.
TEST(ClockTest, UpdatePosition) {
  Clock clock(kSampleRate);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);

  clock.UpdatePosition(kSampleRate);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);

  clock.SetTempo(kTempo);

  clock.UpdatePosition(kSampleRate);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), kTempo / kSecondsFromMinutes);
}

}  // namespace
}  // namespace barelyapi
