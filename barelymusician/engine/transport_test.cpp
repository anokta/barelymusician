#include "barelymusician/engine/transport.h"

#include <string>
#include <tuple>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::ElementsAre;

// Tests that transport sets its tempo as expected.
TEST(TransportTest, SetTempo) {
  const double kTempo = 1.5;

  Transport transport;
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);

  transport.SetTempo(kTempo);
  EXPECT_DOUBLE_EQ(transport.GetTempo(), kTempo);
}

// Tests that transport sets its current position as expected.
TEST(TransportTest, SetPosition) {
  const double kPosition = 2.75;

  Transport transport;
  EXPECT_EQ(transport.GetPosition(), 0.0);
  EXPECT_EQ(transport.GetTimestamp(0.0), 0.0);

  transport.SetPosition(kPosition);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), kPosition);
  EXPECT_EQ(transport.GetTimestamp(kPosition), 0.0);
}

// Tests that updating transport triggers beat callback as expected.
TEST(TransportTest, SetBeatCallback) {
  Transport transport;

  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 0.0);

  std::vector<std::tuple<std::string, std::vector<double>>> callback_values;
  transport.SetBeatCallback([&](double position, double timestamp) {
    callback_values.emplace_back("Beat",
                                 std::vector<double>{position, timestamp});
    if (position == 2.0) {
      // Halve the playback tempo.
      transport.SetTempo(0.5);
    } else if (position == 4.0) {
      // Jump to the fifth beat.
      transport.SetPosition(5.0);
    }
  });
  const auto update_callback = [&](double begin_position, double end_position) {
    callback_values.emplace_back(
        "Update", std::vector<double>{
                      begin_position, transport.GetTimestamp(begin_position),
                      end_position, transport.GetTimestamp(end_position)});
  };

  transport.Update(10.0, update_callback);
  EXPECT_TRUE(callback_values.empty());

  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 10.0);

  transport.Start();
  transport.Update(20.0, update_callback);
  EXPECT_THAT(
      callback_values,
      ElementsAre(
          std::tuple("Beat", std::vector<double>{0.0, 10.0}),
          std::tuple("Update", std::vector<double>{0.0, 10.0, 1.0, 11.0}),
          std::tuple("Beat", std::vector<double>{1.0, 11.0}),
          std::tuple("Update", std::vector<double>{1.0, 11.0, 2.0, 12.0}),
          std::tuple("Beat", std::vector<double>{2.0, 12.0}),
          std::tuple("Update", std::vector<double>{2.0, 12.0, 3.0, 14.0}),
          std::tuple("Beat", std::vector<double>{3.0, 14.0}),
          std::tuple("Update", std::vector<double>{3.0, 14.0, 4.0, 16.0}),
          std::tuple("Beat", std::vector<double>{4.0, 16.0}),
          std::tuple("Update", std::vector<double>{5.0, 16.0, 6.0, 18.0}),
          std::tuple("Beat", std::vector<double>{6.0, 18.0}),
          std::tuple("Update", std::vector<double>{6.0, 18.0, 7.0, 20.0})));

  EXPECT_TRUE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 0.5);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 7.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 20.0);
}

// Tests that transport updates its state as expected.
TEST(TransportTest, Update) {
  Transport transport;
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 0.0);

  double update_begin_position = 0.0;
  double update_end_position = 0.0;
  const auto update_callback = [&](double begin_position, double end_position) {
    update_begin_position = begin_position;
    update_end_position = end_position;
  };
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 0.0);

  transport.Update(1.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 1.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 0.0);

  transport.Start();
  EXPECT_TRUE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);

  transport.Update(2.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 1.0);

  EXPECT_DOUBLE_EQ(transport.GetTimestamp(1.0), 2.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(0.0), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(2.0), 3.0);

  transport.Update(2.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 2.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 1.0);

  EXPECT_DOUBLE_EQ(transport.GetTimestamp(1.0), 2.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(0.0), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(2.0), 3.0);

  transport.SetTempo(1.5);
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.5);

  transport.Update(3.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 3.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);

  EXPECT_DOUBLE_EQ(transport.GetTimestamp(2.5), 3.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(1.0), 2.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(4.0), 4.0);

  transport.Stop();
  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.5);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 3.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);

  transport.Update(4.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 4.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);
}

}  // namespace
}  // namespace barelyapi
