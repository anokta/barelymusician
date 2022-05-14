#include "barelymusician/engine/clock.h"

#include <limits>

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that clock returns beats and seconds as expected.
TEST(ClockTest, GetBeatsSeconds) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 120.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(1.0), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(1.0), 0.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(-1.0), -2.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(-1.0), -0.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(2.5), 5.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(2.5), 1.25);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(clock.GetBeats(10.0), 20.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(10.0), 5.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(10.0)), 10.0);

  // Increase tempo.
  clock.SetTempo(150.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(1.0), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(1.0), 0.4);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(-1.0), -2.5);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(-1.0), -0.4);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(2.5), 6.25);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(2.5), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(clock.GetBeats(10.0), 25.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(10.0), 4.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(10.0)), 10.0);

  // Decrease tempo.
  clock.SetTempo(60.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(1.0), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(1.0)), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(1.0), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(1.0)), 1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(-1.0), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(-1.0)), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(-1.0), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(-1.0)), -1.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(2.5), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(2.5)), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(2.5), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(2.5)), 2.5);

  EXPECT_DOUBLE_EQ(clock.GetBeats(10.0), 10.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(10.0)), 10.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(10.0), 10.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(10.0)), 10.0);

  // Set tempo to zero.
  clock.SetTempo(0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(0.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(0.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(0.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(1.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(1.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(1.0), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(1.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(-1.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(-1.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(-1.0),
                   std::numeric_limits<double>::lowest());
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(-1.0)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(2.5), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(2.5)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(2.5), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(2.5)), 0.0);

  EXPECT_DOUBLE_EQ(clock.GetBeats(10.0), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetBeats(clock.GetSeconds(10.0)), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetSeconds(10.0), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(clock.GetSeconds(clock.GetBeats(10.0)), 0.0);
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

}  // namespace
}  // namespace barely::internal
