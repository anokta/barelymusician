#include "engine/engine_controller.h"

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

#include "engine/engine_state.h"
#include "engine/message.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Optional;
using ::testing::Pair;

// Tests that a single performer is acquired and released as expected.
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
  auto& task = engine->GetTask(task_index);

  // Start the performer with a tempo of one beat per second.
  engine->tempo = 60.0;
  EXPECT_FALSE(performer.is_playing);
  EXPECT_FALSE(task.is_active);
  performer.Start();
  EXPECT_TRUE(performer.is_playing);
  EXPECT_FALSE(task.is_active);

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(1.0, 0)));
  controller.Update(1.0);
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.0, 0)));
  EXPECT_DOUBLE_EQ(performer.position, 1.0);
  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.0, 0)));
  controller.Update(2.5);
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  EXPECT_DOUBLE_EQ(performer.position, 2.5);
  EXPECT_TRUE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kBegin);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Update the timestamp just past the task, which should not be active anymore.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  controller.Update(3.0);
  EXPECT_FALSE(performer.GetNextTaskKey().has_value());
  EXPECT_DOUBLE_EQ(performer.position, 3.0);
  EXPECT_FALSE(task.is_active);
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);
}

}  // namespace
}  // namespace barely
