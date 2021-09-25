#include "barelymusician/engine/clock.h"

#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::ElementsAre;

constexpr double kTempo = 1.5;

// Tests that the clock sets its tempo as expected.
TEST(ClockTest, SetTempo) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);

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

// Tests that updating the clock position tirggers the corresponding callbacks
// as expected.
TEST(ClockTest, SetCallbacks) {
  Clock clock;

  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.0);

  std::vector<std::tuple<std::string, double, double>> callback_values;
  clock.SetBeatCallback([&](double beat, double timestamp) {
    callback_values.emplace_back("Beat", beat, timestamp);
    if (beat == 2.0) {
      // Reverse the playback direction.
      clock.SetTempo(-1.0);
    } else if (beat == -1.0) {
      // Jump to the fifth beat.
      clock.SetPosition(5.0);
    }
  });
  clock.SetUpdateCallback([&](double begin_position, double end_position) {
    callback_values.emplace_back("Update", begin_position, end_position);
  });

  clock.UpdatePosition(10.0);
  EXPECT_TRUE(callback_values.empty());

  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 10.0);

  clock.SetTempo(1.0);
  clock.UpdatePosition(16);
  EXPECT_THAT(
      callback_values,
      ElementsAre(
          std::tuple("Beat", 0.0, 10.0), std::tuple("Update", 0.0, 1.0),
          std::tuple("Beat", 1.0, 11.0), std::tuple("Update", 1.0, 2.0),
          std::tuple("Beat", 2.0, 12.0), std::tuple("Update", 2.0, 1.0),
          std::tuple("Beat", 1.0, 13.0), std::tuple("Update", 1.0, 0.0),
          std::tuple("Beat", 0.0, 14.0), std::tuple("Update", 0.0, -1.0),
          std::tuple("Beat", -1.0, 15.0), std::tuple("Update", 5.0, 4.0)));

  EXPECT_DOUBLE_EQ(clock.GetTempo(), -1.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 4.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 16.0);
}

// Tests that the clock updates its position as expected.
TEST(ClockTest, UpdatePosition) {
  Clock clock;
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 0.0);

  clock.UpdatePosition(1.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 1.0);

  clock.SetTempo(1.0);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(0.0), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(1.0), 2.0);

  clock.UpdatePosition(2.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(2.0), 3.0);

  clock.UpdatePosition(2.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(1.0), 2.0);

  clock.SetTempo(1.5);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 1.5);

  clock.UpdatePosition(3.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 3.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 3.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(1.0), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(4.0), 4.0);

  clock.SetTempo(-1.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 2.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 3.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestampAtPosition(4.0), 1.5);

  clock.UpdatePosition(4.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(clock.GetPositionAtNextBeat(), 1.0);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 4.0);

  clock.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(clock.GetTempo(), 0.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 4.0);

  clock.UpdatePosition(5.0);
  EXPECT_DOUBLE_EQ(clock.GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(clock.GetTimestamp(), 5.0);
}

}  // namespace
}  // namespace barelyapi
