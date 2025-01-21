#include "engine/performer.h"

#include <array>
#include <functional>
#include <utility>

#include "barelymusician.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::Optional;

// Tests that the performer processs a single task as expected.
TEST(PerformerTest, ProcessSingleTask) {
  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetNextDuration().has_value());

  // Create a task.
  int task_process_begin_count = 0;
  int task_process_end_count = 0;
  int task_process_update_count = 0;
  std::function<void(TaskState)> process_callback = [&](TaskState state) {
    switch (state) {
      case TaskState::kBegin:
        ++task_process_begin_count;
        break;
      case TaskState::kEnd:
        ++task_process_end_count;
        break;
      case TaskState::kUpdate:
        ++task_process_update_count;
        break;
    }
  };
  auto* task = performer.CreateTask(0.25, 0.6,
                                    {
                                        [](BarelyTaskState state, void* user_data) {
                                          (*static_cast<std::function<void(TaskState)>*>(
                                              user_data))(static_cast<TaskState>(state));
                                        },
                                        &process_callback,
                                    });

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetNextDuration().has_value());
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);
  EXPECT_EQ(task_process_update_count, 0);

  // Start the performer.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.25));
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);
  EXPECT_EQ(task_process_update_count, 0);

  // Process the task.
  performer.Update(0.25);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.0));
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);
  EXPECT_EQ(task_process_update_count, 0);

  performer.ProcessAllTasksAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.6));
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 0);
  EXPECT_EQ(task_process_update_count, 0);

  performer.Update(0.6);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.85);
  EXPECT_FALSE(performer.GetNextDuration().has_value());
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 1);
  EXPECT_EQ(task_process_update_count, 0);

  // Set looping on.
  performer.SetLooping(true);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.4));

  // Process the next task with a loop back.
  performer.Update(0.4);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.0));
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 1);
  EXPECT_EQ(task_process_update_count, 0);

  performer.ProcessAllTasksAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.6));
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 1);
  EXPECT_EQ(task_process_update_count, 0);

  // Update the task position.
  task->SetPosition(0.75);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.25);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.5));
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 2);
  EXPECT_EQ(task_process_update_count, 0);

  // Process the task with the updated position.
  performer.Update(0.5);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.0));
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 2);
  EXPECT_EQ(task_process_update_count, 0);

  performer.ProcessAllTasksAtPosition();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.75);
  EXPECT_THAT(performer.GetNextDuration(), Optional(0.25));
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 2);
  EXPECT_EQ(task_process_update_count, 0);

  // Update the position while task is still active.
  performer.Update(0.05);
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.8);
  EXPECT_DOUBLE_EQ(*performer.GetNextDuration(), 0.2);
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 2);
  EXPECT_EQ(task_process_update_count, 1);

  // Stop the performer.
  performer.Stop();
  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.8);
  EXPECT_FALSE(performer.GetNextDuration().has_value());
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 3);
  EXPECT_EQ(task_process_update_count, 1);
}

// Tests that the performer processs multiple tasks as expected.
TEST(PerformerTest, ProcessMultipleTasks) {
  constexpr int kTaskCount = 4;

  Performer performer;

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetNextDuration().has_value());

  // Create tasks.
  std::array<std::pair<std::function<void(TaskState)>, bool>, kTaskCount> task_callbacks;
  for (int i = 1; i <= kTaskCount; ++i) {
    task_callbacks[i - 1] = {
        [&, i](TaskState state) {
          if (state == TaskState::kBegin) {
            task_callbacks[i - 1].second = true;
          } else if (state == TaskState::kEnd) {
            task_callbacks[i - 1].second = false;
          }
        },
        false,
    };
    performer.CreateTask(static_cast<double>(i), 1.0,
                         {[](BarelyTaskState state, void* user_data) {
                            (*static_cast<std::function<void(TaskState)>*>(user_data))(
                                static_cast<TaskState>(state));
                          },
                          &task_callbacks[i - 1].first});
  }

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_FALSE(performer.GetNextDuration().has_value());
  for (auto& [_, is_active] : task_callbacks) {
    EXPECT_FALSE(is_active);
  }

  // Start playback.
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 0.0);
  EXPECT_THAT(performer.GetNextDuration(), Optional(1.0));
  for (auto& [_, is_active] : task_callbacks) {
    EXPECT_FALSE(is_active);
  }

  // Process tasks.
  for (int i = 1; i <= kTaskCount + 1; ++i) {
    ASSERT_THAT(performer.GetNextDuration(), Optional(1.0));

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    performer.Update(*performer.GetNextDuration());
    EXPECT_DOUBLE_EQ(performer.GetPosition(), static_cast<double>(i));

    performer.ProcessAllTasksAtPosition();
    for (int j = 1; j <= kTaskCount; ++j) {
      EXPECT_EQ(task_callbacks[j - 1].second, i == j);
    }
  }

  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), kTaskCount + 1);
  EXPECT_FALSE(performer.GetNextDuration().has_value());
  for (int i = 1; i <= kTaskCount; ++i) {
    EXPECT_EQ(task_callbacks[i - 1].second, false);
  }
}

// Tests that the performer sets its current position as expected.
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
