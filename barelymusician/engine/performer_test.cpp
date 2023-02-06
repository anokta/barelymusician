#include "barelymusician/engine/performer.h"

#include <limits>
#include <vector>

#include "barelymusician/engine/id.h"
#include "barelymusician/engine/number.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that performer triggers multiple tasks as expected.
TEST(PerformerTest, TriggerMultipleTasks) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());

  // Add tasks.
  std::vector<double> positions;
  for (Integer i = 1; i <= 4; ++i) {
    performer.AddTask(Id{i}, static_cast<double>(i), [&, i]() {
      const double position = performer.GetPosition();
      EXPECT_DOUBLE_EQ(position, static_cast<double>(i));
      positions.push_back(position);
    });
  }
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());
  EXPECT_TRUE(positions.empty());

  // Start playback.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 1.0);
  EXPECT_TRUE(positions.empty());

  // Trigger tasks.
  std::vector<double> expected_positions;
  for (Integer i = 1; i <= 4; ++i) {
    const double expected_position = static_cast<double>(i);
    expected_positions.push_back(expected_position);
    EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 1.0);

    performer.Update(performer.GetDurationToNextTask());
    EXPECT_DOUBLE_EQ(performer.GetPosition(), expected_position);

    performer.TriggerAllTasksAtCurrentPosition();
    EXPECT_EQ(positions, expected_positions);
  }

  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 4.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());
}

// Tests that performer triggers a single task as expected.
TEST(PerformerTest, TriggerSingleTask) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());

  // Add task.
  Integer task_trigger_count = 0;
  EXPECT_TRUE(performer.AddTask(Id{1}, 0.25, [&]() {
    EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
    ++task_trigger_count;
  }));
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(task_trigger_count, 0);

  // Start playback.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 0.25);
  EXPECT_EQ(task_trigger_count, 0);

  // Trigger task.
  performer.Update(0.25);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 0.0);
  EXPECT_EQ(task_trigger_count, 0);

  performer.TriggerAllTasksAtCurrentPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(task_trigger_count, 1);

  // Set looping on.
  performer.SetLooping(true);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 1.0);

  // Trigger next task with a loop back.
  performer.Update(1.0);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 0.0);
  EXPECT_EQ(task_trigger_count, 1);

  performer.TriggerAllTasksAtCurrentPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 1.0);
  EXPECT_EQ(task_trigger_count, 2);

  // Update task position and callback.
  EXPECT_TRUE(performer.SetTaskPosition(Id{1}, 0.75));
  EXPECT_TRUE(performer.SetTaskCallback(Id{1}, [&]() {
    EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
    --task_trigger_count;
  }));
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 0.5);
  EXPECT_EQ(task_trigger_count, 2);

  // Trigger task with the updated position and callback.
  performer.Update(0.5);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 0.0);
  EXPECT_EQ(task_trigger_count, 2);

  performer.TriggerAllTasksAtCurrentPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(), 1.0);
  EXPECT_EQ(task_trigger_count, 1);

  // Stop playback.
  performer.Stop();
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_DOUBLE_EQ(performer.GetDurationToNextTask(),
                   std::numeric_limits<double>::max());
  EXPECT_EQ(task_trigger_count, 1);
}

// TODO(#108): Add `TriggerOneOffTasks` test (at minimum).

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
