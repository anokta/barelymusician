#include "barelymusician/engine/transport.h"

#include <string>
#include <tuple>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;

// Tests that updating transport triggers beat callback as expected.
TEST(TransportTest, SetBeatCallback) {
  Transport transport;

  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);

  std::vector<std::tuple<std::string, std::vector<double>>> callback_values;
  transport.SetBeatCallback([&](double position) {
    callback_values.emplace_back("Beat", std::vector<double>{position});
    if (position == 2.0) {
      // Jump to the fifth beat.
      transport.SetPosition(5.0);
    }
  });
  const auto update_callback = [&](double begin_position, double end_position) {
    callback_values.emplace_back(
        "Update", std::vector<double>{begin_position, end_position});
  };

  transport.Update(5.0, update_callback);
  EXPECT_TRUE(callback_values.empty());
  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);

  // Start playback.
  transport.Start();
  transport.Update(5.0, update_callback);
  EXPECT_THAT(callback_values,
              ElementsAre(std::tuple("Beat", std::vector<double>{0.0}),
                          std::tuple("Update", std::vector<double>{0.0, 1.0}),
                          std::tuple("Beat", std::vector<double>{1.0}),
                          std::tuple("Update", std::vector<double>{1.0, 2.0}),
                          std::tuple("Beat", std::vector<double>{2.0}),
                          std::tuple("Update", std::vector<double>{5.0, 6.0}),
                          std::tuple("Beat", std::vector<double>{6.0}),
                          std::tuple("Update", std::vector<double>{6.0, 7.0}),
                          std::tuple("Beat", std::vector<double>{7.0}),
                          std::tuple("Update", std::vector<double>{7.0, 8.0})));
  EXPECT_TRUE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 8.0);
}

// Tests that transport sets its current position as expected.
TEST(TransportTest, SetPosition) {
  Transport transport;
  EXPECT_EQ(transport.GetPosition(), 0.0);

  transport.SetPosition(2.75);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.75);

  transport.SetPosition(1.10);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.10);
}

// Tests that transport updates its state as expected.
TEST(TransportTest, Update) {
  Transport transport;
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 0.0);

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
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 0.0);

  transport.Start();
  EXPECT_TRUE(transport.IsPlaying());

  transport.Update(1.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 1.0);

  transport.Update(0.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(update_begin_position, 0.0);
  EXPECT_DOUBLE_EQ(update_end_position, 1.0);

  transport.Update(1.5, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);

  transport.Stop();
  EXPECT_FALSE(transport.IsPlaying());
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);

  transport.Update(1.0, update_callback);
  EXPECT_DOUBLE_EQ(transport.GetPosition(), 2.5);
  EXPECT_DOUBLE_EQ(update_begin_position, 2.0);
  EXPECT_DOUBLE_EQ(update_end_position, 2.5);
}

}  // namespace
}  // namespace barely::internal
