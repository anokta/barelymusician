#include "engine/engine_controller.h"

#include <barelymusician.h>

#include <cstdint>
#include <functional>
#include <memory>

#include "engine/engine_state.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

TEST(EngineControllerTest, AcquireReleasePerformer) {
  auto engine = std::make_unique<EngineState>();
  EngineController controller(*engine);

  // Create a performer.
  const uint32_t performer_index = controller.performer_controller().Acquire();
  auto& performer = engine->GetPerformer(performer_index);

  // Create a task.
  barely::TaskEventType task_event_type = barely::TaskEventType::kEnd;
  double task_position = 0.0;
  std::function<void(barely::TaskEventType)> process_callback = [&](barely::TaskEventType type) {
    task_event_type = type;
    task_position = performer.position;
  };

  const uint32_t task_index = controller.performer_controller().AcquireTask(
      performer_index, 1.0, 2.0, 0,
      [](BarelyTaskEventType type, void* user_data) {
        (*static_cast<std::function<void(barely::TaskEventType)>*>(user_data))(
            static_cast<barely::TaskEventType>(type));
      },
      &process_callback);
  const auto& task = engine->GetTask(task_index);

  // Start the performer with a tempo of one beat per second.
  engine->tempo = 60.0;
  EXPECT_FALSE(performer.is_playing);
  EXPECT_FALSE(task.is_active);
  controller.performer_controller().Start(performer_index);
  EXPECT_TRUE(performer.is_playing);
  EXPECT_FALSE(task.is_active);

  double duration = 10.0;
  int32_t max_priority = INT32_MIN;

  // Update the timestamp just before the task, which should not be triggered.
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 1.0);
  EXPECT_EQ(max_priority, 0);

  controller.Update(1.0);

  duration = 10.0;
  max_priority = INT32_MIN;
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.0);
  EXPECT_EQ(max_priority, 0);

  EXPECT_DOUBLE_EQ(performer.position, 1.0);
  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  duration = 10.0;
  max_priority = INT32_MIN;
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.0);
  EXPECT_EQ(max_priority, 0);

  controller.Update(2.5);

  duration = 10.0;
  max_priority = INT32_MIN;
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.5);
  EXPECT_EQ(max_priority, 0);

  EXPECT_DOUBLE_EQ(performer.position, 2.5);
  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kBegin);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Update the timestamp just past the task, which should not be active anymore.
  duration = 10.0;
  max_priority = INT32_MIN;
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 0.5);
  EXPECT_EQ(max_priority, 0);

  controller.Update(3.0);

  duration = 10.0;
  max_priority = INT32_MIN;
  controller.performer_controller().GetNextTaskEvent(duration, max_priority);
  EXPECT_DOUBLE_EQ(duration, 10.0);
  EXPECT_EQ(max_priority, INT32_MIN);

  EXPECT_DOUBLE_EQ(performer.position, 3.0);
  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);
}

}  // namespace
}  // namespace barely
