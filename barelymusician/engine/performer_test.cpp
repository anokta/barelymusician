#include "barelymusician/engine/performer.h"

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/engine/id.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::Optional;
using ::testing::Pair;

// Tests that performer processs a single task as expected.
TEST(PerformerTest, ProcessSingleTask) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());

  // Create a task definition.
  int task_process_count = 0;
  auto definition = TaskDefinition{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) {
        int& count = *static_cast<int*>(*state);
        ++count;
      },
  };

  // Create a recurring task.
  performer.CreateTask(Id{1}, definition, 0.25, 0, &task_process_count,
                       /*is_one_off=*/false);
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 0);

  // Start the performer.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.25, 0)));
  EXPECT_EQ(task_process_count, 0);

  // Process the task.
  performer.Update(0.25);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.0, 0)));
  EXPECT_EQ(task_process_count, 0);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 1);

  // Set looping on.
  performer.SetLooping(true);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, 0)));

  // Process the next task with a loop back.
  performer.Update(1.0);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.0, 0)));
  EXPECT_EQ(task_process_count, 1);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, 0)));
  EXPECT_EQ(task_process_count, 2);

  // Update the task position.
  EXPECT_TRUE(performer.SetTaskPosition(Id{1}, 0.75).IsOk());
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.5, 0)));
  EXPECT_EQ(task_process_count, 2);

  // Process the task with the updated position.
  performer.Update(0.5);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.0, 0)));
  EXPECT_EQ(task_process_count, 2);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, 0)));
  EXPECT_EQ(task_process_count, 3);

  // Stop the performer.
  performer.Stop();
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 3);
}

// Tests that performer processs multiple tasks as expected.
TEST(PerformerTest, ProcessMultipleTasks) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());

  // Create tasks.
  std::vector<double> positions;
  const auto create_task_fn = [&](int i) {
    std::function<void()> callback = [&, i]() {
      const double position = performer.GetPosition();
      EXPECT_DOUBLE_EQ(position, static_cast<double>(i));
      positions.push_back(position);
    };
    performer.CreateTask(
        Id{i},
        TaskDefinition(
            [](void** state, void* user_data) {
              *state = new std::function<void()>(
                  std::move(*static_cast<std::function<void()>*>(user_data)));
            },
            [](void** state) {
              delete static_cast<std::function<void()>*>(*state);
            },
            [](void** state) {
              (*static_cast<std::function<void()>*>(*state))();
            }),
        static_cast<double>(i), 4 - i, static_cast<void*>(&callback),
        /*is_one_off=*/false);
  };
  for (int i = 1; i <= 4; ++i) {
    create_task_fn(i);
  }

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_TRUE(positions.empty());

  // Start playback.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, 3)));
  EXPECT_TRUE(positions.empty());

  // Process tasks.
  std::vector<double> expected_positions;
  for (int i = 1; i <= 4; ++i) {
    const double expected_position = static_cast<double>(i);
    expected_positions.push_back(expected_position);
    EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, 4 - i)));

    performer.Update(performer.GetDurationToNextTask()->first);
    EXPECT_DOUBLE_EQ(performer.GetPosition(), expected_position);

    performer.ProcessNextTaskAtPosition();
    EXPECT_EQ(positions, expected_positions);
  }

  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 4.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
}

// TODO(#108): Add `ProcessOneOffTasks` test (at minimum).

// Tests that performer sets its current position as expected.
TEST(PerformerTest, SetPosition) {
  Performer performer;
  EXPECT_EQ(performer.GetPosition(), 0.0);

  performer.SetPosition(2.75);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 2.75);

  performer.SetPosition(1.25);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.25);

  // Set looping on which should wrap the current position back.
  performer.SetLooping(true);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);

  performer.SetPosition(3.5);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.5);

  // Set loop begin position.
  performer.SetLoopBeginPosition(0.75);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.5);

  // Set loop length.
  performer.SetLoopLength(2.0);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.5);

  performer.SetPosition(4.0f);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 2.0);

  // Resetting back position before the loop should still be okay.
  performer.SetPosition(0.25);
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
}

}  // namespace
}  // namespace barely::internal
