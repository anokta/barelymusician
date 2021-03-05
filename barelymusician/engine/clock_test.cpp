#include "barelymusician/engine/clock.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr double kTempo = 1.5;

// Tests that the clock sets its tempo as expected.
TEST(ClockTest, SetTempo) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 1.0);

  clock.SetTempo(kTempo);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), kTempo);
}

// Tests that the clock sets its current position as expected.
TEST(ClockTest, SetPosition) {
  const double kPosition = 2.75f;

  Clock clock;
  EXPECT_EQ(clock.GetPosition(), 0.0);

  clock.SetPosition(kPosition);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), kPosition);
}

// Tests that the clock updates its state as expected.
TEST(ClockTest, Update) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.0);
  EXPECT_FALSE(clock.IsPlaying());

  clock.Update(1.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 1.0);

  clock.Start();
  EXPECT_TRUE(clock.IsPlaying());

  clock.Update(2.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(2.0), 3.0);

  clock.Update(2.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(1.0), 2.0);

  clock.SetTempo(kTempo);

  clock.Update(3.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0 + kTempo);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 3.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(1.0), 2.0);

  clock.Stop();

  clock.Update(4.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0 + kTempo);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 4.0);
}

}  // namespace
}  // namespace barelyapi
