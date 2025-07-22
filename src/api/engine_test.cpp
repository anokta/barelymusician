#include "api/engine.h"

#include <barelymusician.h>

#include <functional>

#include "api/performer.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace {

using ::testing::Optional;
using ::testing::Pair;

constexpr int kSampleRate = 48000;
constexpr float kReferenceFrequency = 440.0f;

// Tests that a single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  BarelyEngine engine(kSampleRate, kReferenceFrequency);

  // Create a performer.
  BarelyPerformer performer(engine);

  // Create a task.
  barely::TaskState task_state = barely::TaskState::kEnd;
  double task_position = 0.0;
  std::function<void(barely::TaskState)> process_callback = [&](barely::TaskState state) {
    task_state = state;
    task_position = performer.GetPosition();
  };
  const BarelyTask task(performer, 1.0, 2.0, 0,
                        {
                            [](BarelyTaskState state, void* user_data) {
                              (*static_cast<std::function<void(barely::TaskState)>*>(user_data))(
                                  static_cast<barely::TaskState>(state));
                            },
                            &process_callback,
                        });

  // Start the performer with a tempo of one beat per second.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer.IsPlaying());
  EXPECT_FALSE(task.IsActive());
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());
  EXPECT_FALSE(task.IsActive());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(1.0, 0)));
  engine.Update(1.0);
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.0, 0)));
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.0);
  EXPECT_FALSE(task.IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.0, 0)));
  engine.Update(2.5);
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 2.5);
  EXPECT_TRUE(task.IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kBegin);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Update the timestamp just past the task, which should not be active anymore.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  engine.Update(3.0);
  EXPECT_FALSE(performer.GetNextTaskKey().has_value());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 3.0);
  EXPECT_FALSE(task.IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);
}

// Tests that the engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  BarelyEngine engine(kSampleRate, kReferenceFrequency);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);

  engine.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
