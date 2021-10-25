#include "barelymusician/engine/transport.h"

#include <tuple>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::ElementsAre;

constexpr double kTempo = 1.5;

// Tests that the transport sets its tempo as expected.
TEST(TransportTest, SetTempo) {
  Transport transport;
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);

  transport.SetTempo(kTempo);
  EXPECT_DOUBLE_EQ(transport.GetTempo(), kTempo);
}

// Tests that the transport sets its current position as expected.
TEST(TransportTest, SetPosition) {
  const double kPosition = 2.75f;

  Transport transport;
  EXPECT_EQ(transport.GetPosition(), 0.0);

  transport.SetPosition(kPosition);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), kPosition);
}

// Tests that updating the transport position tirggers the corresponding
// callbacks as expected.
TEST(TransportTest, SetCallbacks) {
  Transport transport;

  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 0.0);

  std::vector<std::tuple<std::string, std::vector<double>>> callback_values;
  transport.SetBeatCallback([&](double position) {
    callback_values.emplace_back(
        "Beat", std::vector<double>{position, transport.GetTimestamp()});
    if (position == 2.0) {
      // Halve the playback tempo.
      transport.SetTempo(0.5);
    } else if (position == 4.0) {
      // Jump to the fifth beat.
      transport.SetPosition(5.0);
    }
  });
  transport.SetUpdateCallback(
      [&](double begin_position, double end_position,
          const Transport::GetTimestampFn& get_timestamp_fn) {
        callback_values.emplace_back(
            "Update", std::vector<double>{
                          begin_position, get_timestamp_fn(begin_position),
                          end_position, get_timestamp_fn(end_position)});
      });

  transport.Update(10.0);
  EXPECT_TRUE(callback_values.empty());

  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 10.0);

  transport.Start();
  transport.Update(20.0);
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

// Tests that the transport updates its internal state as expected.
TEST(TransportTest, Update) {
  Transport transport;
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 0.0);

  transport.Update(1.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 1.0);

  transport.Start();
  EXPECT_TRUE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.0);

  transport.Update(2.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 2.0);

  transport.Update(2.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 2.0);

  transport.SetTempo(1.5);
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.5);

  transport.Update(3.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 3.0);

  transport.Stop();
  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetTempo(), 1.5);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 3.0);

  transport.Update(4.0);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(transport.GetTimestamp(), 4.0);
}

}  // namespace
}  // namespace barelyapi
