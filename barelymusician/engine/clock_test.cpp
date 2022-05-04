#include "barelymusician/engine/clock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that clock returns duration as expected.
TEST(ClockTest, GetDuration) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 120.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetDuration(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(1.0), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(2.0), 4.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(10.0), 20.0);

  // Increase tempo.
  clock.SetTempo(150.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(1.0), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetDuration(2.5), 6.25);
  EXPECT_DOUBLE_EQ(clock.GetDuration(10.0), 25.0);

  // Update timestamp.
  clock.SetTimestamp(2.5);
  EXPECT_DOUBLE_EQ(clock.GetDuration(2.5), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(3.5), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetDuration(5.0), 6.25);
  EXPECT_DOUBLE_EQ(clock.GetDuration(12.5), 25.0);

  // Decrease tempo.
  clock.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(2.5), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(3.5), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetDuration(5.0), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetDuration(12.5), 10.0);
}

// Tests that clock sets its tempo as expected.
TEST(ClockTest, SetTempo) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 120.0);

  clock.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 200.0);

  clock.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);
}

// Tests that clock sets its timestamp as expected.
TEST(ClockTest, SetTimestamp) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.0);

  clock.SetTimestamp(1.5);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 1.5);

  clock.SetTimestamp(0.1);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.1);
}

}  // namespace
}  // namespace barely::internal
