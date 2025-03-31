#include "private/engine.h"

#include <array>
#include <functional>

#include "barelymusician.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include "private/performer.h"

namespace {

using ::testing::Optional;

constexpr int kSampleRate = 48000;

// Tests that the engine converts between beats and seconds as expected.
TEST(EngineTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  BarelyEngine engine(kSampleRate);
  engine.SetTempo(kTempo);

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(engine.BeatsToSeconds(kBeats[i]), kSeconds[i]);
    EXPECT_DOUBLE_EQ(engine.SecondsToBeats(kSeconds[i]), kBeats[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(engine.BeatsToSeconds(engine.SecondsToBeats(kSeconds[i])), kSeconds[i]);
    EXPECT_DOUBLE_EQ(engine.SecondsToBeats(engine.BeatsToSeconds(kBeats[i])), kBeats[i]);
  }
}

// Tests that a single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  BarelyEngine engine(kSampleRate);

  // Create a performer.
  barely::PerformerImpl* performer = engine.CreatePerformer();

  // Create a task.
  barely::TaskState task_state = barely::TaskState::kEnd;
  double task_position = 0.0;
  std::function<void(barely::TaskState)> process_callback = [&](barely::TaskState state) {
    // `kUpdate` can only be called after `kBegin`, and not the other way around.
    EXPECT_TRUE(task_state != barely::TaskState::kBegin || state == barely::TaskState::kUpdate);
    EXPECT_TRUE(task_state != barely::TaskState::kUpdate || state != barely::TaskState::kBegin);
    task_state = state;
    task_position = performer->GetPosition();
  };
  auto* task = performer->CreateTask(1.0, 2.0,
                                     {
                                         [](BarelyTaskState state, void* user_data) {
                                           (*static_cast<std::function<void(barely::TaskState)>*>(
                                               user_data))(static_cast<barely::TaskState>(state));
                                         },
                                         &process_callback,
                                     });

  // Start the performer with a tempo of one beat per second.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer->IsPlaying());
  EXPECT_FALSE(task->IsActive());
  performer->Start();
  EXPECT_TRUE(performer->IsPlaying());
  EXPECT_FALSE(task->IsActive());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer->GetNextDuration(), Optional(1.0));
  engine.Update(1.0);
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.0));
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.0);
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.0));
  engine.Update(2.5);
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.5));
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 2.5);
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kUpdate);
  EXPECT_DOUBLE_EQ(task_position, 2.5);

  // Update the timestamp just past the task, which should not be active anymore.
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.5));
  engine.Update(3.0);
  EXPECT_FALSE(performer->GetNextDuration().has_value());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 3.0);
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_state, barely::TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);

  // Remove the performer.
  engine.DestroyPerformer(performer);
}

// Tests that the engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  BarelyEngine engine(kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);

  engine.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
