#include "barelymusician/internal/performer.h"

#include <functional>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "barelymusician/internal/task.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::Optional;
using ::testing::Pair;

// Tests that the performer processs a single task as expected.
TEST(PerformerTest, ProcessSingleTask) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
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
  Task task(definition, Rational(1, 4), 0, &task_process_count);
  performer.AddTask(task);

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 0);

  // Start the performer.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1, 4), 0)));
  EXPECT_EQ(task_process_count, 0);

  // Process the task.
  performer.Update(Rational(1, 4));
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(0), 0)));
  EXPECT_EQ(task_process_count, 0);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 1);

  // Set looping on.
  performer.SetLooping(true);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), 0)));

  // Process the next task with a loop back.
  performer.Update(1);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(0), 0)));
  EXPECT_EQ(task_process_count, 1);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), 0)));
  EXPECT_EQ(task_process_count, 2);

  // Update the task position.
  performer.SetTaskPosition(task, Rational(3, 4));
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1, 2), 0)));
  EXPECT_EQ(task_process_count, 2);

  // Process the task with the updated position.
  performer.Update(Rational(1, 2));
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(3, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(0), 0)));
  EXPECT_EQ(task_process_count, 2);

  performer.ProcessNextTaskAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(3, 4));
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), 0)));
  EXPECT_EQ(task_process_count, 3);

  // Stop the performer.
  performer.Stop();
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), Rational(3, 4));
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(task_process_count, 3);
}

// Tests that the performer processs multiple tasks as expected.
TEST(PerformerTest, ProcessMultipleTasks) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());

  // Create tasks.
  std::vector<Rational> positions;
  const auto create_task_fn = [&](int i) {
    std::function<void()> callback = [&, i]() {
      const Rational position = performer.GetPosition();
      EXPECT_EQ(position, i);
      positions.push_back(position);
    };
    performer.ScheduleOneOffTask(
        TaskDefinition(
            [](void** state, void* user_data) {
              *state = new std::function<void()>(
                  std::move(*static_cast<std::function<void()>*>(user_data)));
            },
            [](void** state) { delete static_cast<std::function<void()>*>(*state); },
            [](void** state) { (*static_cast<std::function<void()>*>(*state))(); }),
        i, 4 - i, static_cast<void*>(&callback));
  };
  for (int i = 1; i <= 4; ++i) {
    create_task_fn(i);
  }

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_TRUE(positions.empty());

  // Start playback.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), 3)));
  EXPECT_TRUE(positions.empty());

  // Process tasks.
  std::vector<Rational> expected_positions;
  for (int i = 1; i <= 4; ++i) {
    const Rational expected_position = i;
    expected_positions.push_back(expected_position);
    EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), 4 - i)));

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    performer.Update(performer.GetDurationToNextTask()->first);
    EXPECT_EQ(performer.GetPosition(), expected_position);

    performer.ProcessNextTaskAtPosition();
    EXPECT_EQ(positions, expected_positions);
  }

  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_EQ(performer.GetPosition(), 4);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
}

// Tests that the performer sets its current position as expected.
TEST(PerformerTest, SetPosition) {
  Performer performer;
  EXPECT_EQ(performer.GetPosition(), 0);

  performer.SetPosition(Rational(10, 4));
  EXPECT_EQ(performer.GetPosition(), Rational(10, 4));

  performer.SetPosition(Rational(6, 5));
  EXPECT_EQ(performer.GetPosition(), Rational(6, 5));

  // Set looping on which should wrap the current position back.
  performer.SetLooping(true);
  EXPECT_EQ(performer.GetPosition(), Rational(1, 5));

  performer.SetPosition(Rational(11, 2));
  EXPECT_EQ(performer.GetPosition(), Rational(1, 2));

  // Set loop begin position.
  performer.SetLoopBeginPosition(Rational(3, 4));
  EXPECT_EQ(performer.GetPosition(), Rational(1, 2));

  // Set loop length.
  performer.SetLoopLength(2);
  EXPECT_EQ(performer.GetPosition(), Rational(1, 2));

  performer.SetPosition(4);
  EXPECT_EQ(performer.GetPosition(), 2);

  // Resetting back position before the loop should still be okay.
  performer.SetPosition(Rational(1, 4));
  EXPECT_EQ(performer.GetPosition(), Rational(1, 4));
}

}  // namespace
}  // namespace barely::internal
