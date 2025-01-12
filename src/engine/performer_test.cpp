#include "engine/performer.h"

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::Optional;

// Tests that the performer processs a single task as expected.
TEST(PerformerTest, ProcessSingleTask) {
  Performer performer(/*process_order=*/0);

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());

  // Create a task event.
  int task_process_count = 0;
  auto task_event = TaskEvent{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) {
        int& count = *static_cast<int*>(*state);
        ++count;
      },
      &task_process_count,
  };

  // Create a recurring task.
  auto* task = performer.CreateTask(task_event, 0.25);

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 0);

  // Start the performer.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(0.25));
  EXPECT_EQ(task_process_count, 0);

  // Process the task.
  performer.Update(0.25);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(0.0));
  EXPECT_EQ(task_process_count, 0);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 1);

  // Set looping on.
  performer.SetLooping(true);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(1.0));

  // Process the next task with a loop back.
  performer.Update(1.0);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(0.0));
  EXPECT_EQ(task_process_count, 1);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(1.0));
  EXPECT_EQ(task_process_count, 2);

  // Update the task position.
  task->SetPosition(0.75);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(0.5));
  EXPECT_EQ(task_process_count, 2);

  // Process the task with the updated position.
  performer.Update(0.5);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(0.0));
  EXPECT_EQ(task_process_count, 2);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(1.0));
  EXPECT_EQ(task_process_count, 3);

  // Stop the performer.
  performer.Stop();
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 3);
}

// Tests that the performer processs multiple tasks as expected.
TEST(PerformerTest, ProcessMultipleTasks) {
  Performer performer(/*process_order=*/0);

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
        TaskEvent(
            [](void** state, void* user_data) {
              *state = new std::function<void()>(
                  std::move(*static_cast<std::function<void()>*>(user_data)));
            },
            [](void** state) { delete static_cast<std::function<void()>*>(*state); },
            [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
            static_cast<void*>(&callback)),
        static_cast<double>(i));
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
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(1.0));
  EXPECT_TRUE(positions.empty());

  // Process tasks.
  std::vector<double> expected_positions;
  for (int i = 1; i <= 4; ++i) {
    const double expected_position = static_cast<double>(i);
    expected_positions.push_back(expected_position);
    EXPECT_THAT(performer.GetDurationToNextTask(), Optional(1.0));

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    performer.Update(*performer.GetDurationToNextTask());
    EXPECT_DOUBLE_EQ(performer.GetPosition(), expected_position);

    performer.ProcessNextTaskAtPosition();
    EXPECT_EQ(positions, expected_positions);
  }

  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 4.0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
}

// Tests that the performer sets its current position as expected.
TEST(PerformerTest, SetPosition) {
  Performer performer(/*process_order=*/0);
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
