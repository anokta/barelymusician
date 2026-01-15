#include "engine/performer_controller.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>

#include "engine/engine_state.h"
#include "engine/performer_state.h"
#include "engine/task_state.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Pair;

// Tests that a performer processses a single task as expected.
TEST(PerformerControllerTest, ProcessSingleTask) {
  auto engine = std::make_unique<EngineState>();
  PerformerController controller(*engine);

  // Create a performer.
  const uint32_t performer_index = controller.Acquire();
  auto& performer = engine->GetPerformer(performer_index);

  EXPECT_FALSE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  double duration = 1.0;
  int32_t max_priority = INT32_MIN;

  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  // Create a task.
  int task_process_begin_count = 0;
  int task_process_end_count = 0;
  std::function<void(BarelyTaskEventType)> process_callback = [&](BarelyTaskEventType type) {
    if (type == BarelyTaskEventType_kBegin) {
      ++task_process_begin_count;
    } else if (type == BarelyTaskEventType_kEnd) {
      ++task_process_end_count;
    }
  };

  const uint32_t task_index = controller.AcquireTask(
      performer_index, 0.25, 0.6, 0,
      [](BarelyTaskEventType type, void* user_data) {
        (*static_cast<std::function<void(BarelyTaskEventType)>*>(user_data))(type);
      },
      &process_callback);
  const auto& task = engine->GetTask(task_index);

  EXPECT_FALSE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);

  // Start the performer.
  controller.Start(performer_index);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.25);
  EXPECT_EQ(max_priority, 0);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);

  // Process the task.
  controller.Update(0.25);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.25);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.0);
  EXPECT_EQ(max_priority, 0);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 0);
  EXPECT_EQ(task_process_end_count, 0);

  controller.ProcessAllTasksAtPosition(0);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.25);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.6);
  EXPECT_EQ(max_priority, 0);

  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 0);

  controller.Update(0.6);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.85);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 1);

  // Set looping on.
  controller.SetLooping(performer_index, true);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.4);
  EXPECT_EQ(max_priority, 0);

  // Process the next task with a loop back.
  controller.Update(0.4);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.25);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.0);
  EXPECT_EQ(max_priority, 0);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 1);
  EXPECT_EQ(task_process_end_count, 1);

  controller.ProcessAllTasksAtPosition(0);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.25);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.6);
  EXPECT_EQ(max_priority, 0);

  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 1);

  // Update the task position.
  controller.SetTaskPosition(task_index, 0.75);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.25);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.5);
  EXPECT_EQ(max_priority, 0);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 2);

  // Process the task with the updated position.
  controller.Update(0.5);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.75);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.0);
  EXPECT_EQ(max_priority, 0);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 2);
  EXPECT_EQ(task_process_end_count, 2);

  controller.ProcessAllTasksAtPosition(0);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.75);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.25);
  EXPECT_EQ(max_priority, 0);

  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 2);

  // Update the position while task is still active.
  controller.Update(0.05);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.8);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.2);
  EXPECT_EQ(max_priority, 0);

  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 2);

  // Stop the performer.
  controller.Stop(performer_index);
  EXPECT_FALSE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.8);

  duration = 1.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_process_begin_count, 3);
  EXPECT_EQ(task_process_end_count, 3);
}

// Tests that a performer processes multiple tasks as expected.
TEST(PerformerControllerTest, ProcessMultipleTasks) {
  constexpr int kTaskCount = 4;

  auto engine = std::make_unique<EngineState>();
  PerformerController controller(*engine);

  // Create a performer.
  const uint32_t performer_index = controller.Acquire();
  auto& performer = engine->GetPerformer(performer_index);

  EXPECT_FALSE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  double duration = 5.0;
  int32_t max_priority = INT32_MIN;

  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 5.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  // Create tasks.
  std::array<std::pair<std::function<void(BarelyTaskEventType)>, bool>, kTaskCount> task_callbacks;
  for (int i = 0; i < kTaskCount; ++i) {
    task_callbacks[i] = {
        [&, i](BarelyTaskEventType type) {
          if (type == BarelyTaskEventType_kBegin) {
            task_callbacks[i].second = true;
          } else if (type == BarelyTaskEventType_kEnd) {
            task_callbacks[i].second = false;
          }
        },
        false,
    };
    [[maybe_unused]] const uint32_t task_index = controller.AcquireTask(
        performer_index, static_cast<double>(i + 1), 1.0, 0,
        [](BarelyTaskEventType type, void* user_data) {
          (*static_cast<std::function<void(BarelyTaskEventType)>*>(user_data))(type);
        },
        &task_callbacks[i].first);
  }

  EXPECT_FALSE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  duration = 5.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 5.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  for (auto& [_, is_active] : task_callbacks) {
    EXPECT_FALSE(is_active);
  }

  // Start playback.
  controller.Start(performer_index);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, 0.0);

  duration = 5.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, 0);

  for (auto& [_, is_active] : task_callbacks) {
    EXPECT_FALSE(is_active);
  }

  // Process tasks.
  for (int i = 1; i <= kTaskCount + 1; ++i) {
    duration = 5.0;
    max_priority = INT32_MIN;
    controller.GetNextTaskEvent(duration, max_priority);
    ASSERT_DOUBLE_EQ(duration, 1.0);
    ASSERT_EQ(max_priority, 0);

    controller.Update(duration);
    EXPECT_DOUBLE_EQ(performer.position, static_cast<double>(i));

    controller.ProcessAllTasksAtPosition(0);  // beat callback
    controller.ProcessAllTasksAtPosition(0);
    for (int j = 1; j <= kTaskCount; ++j) {
      EXPECT_EQ(task_callbacks[j - 1].second, i == j);
    }
  }

  EXPECT_TRUE(performer.is_playing);
  EXPECT_DOUBLE_EQ(performer.position, kTaskCount + 1);

  duration = 5.0;
  max_priority = INT32_MIN;
  controller.GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 5.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  for (int i = 1; i <= kTaskCount; ++i) {
    EXPECT_EQ(task_callbacks[i - 1].second, false);
  }
}

}  // namespace
}  // namespace barely
