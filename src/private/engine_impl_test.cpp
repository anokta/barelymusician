#include "private/engine_impl.h"

#include <array>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include "private/instrument_impl.h"
#include "private/performer_impl.h"

namespace barely {
namespace {

using ::testing::ElementsAre;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;

// Tests that the engine converts between beats and seconds as expected.
TEST(EngineTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  EngineImpl engine(kSampleRate);
  engine.SetTempo(kTempo);

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(engine.BeatsToSeconds(kBeats[i]), kSeconds[i]);
    EXPECT_DOUBLE_EQ(engine.SecondsToBeats(kSeconds[i]), kBeats[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(engine.BeatsToSeconds(engine.SecondsToBeats(kSeconds[i])), kSeconds[i]);
    EXPECT_DOUBLE_EQ(engine.SecondsToBeats(engine.BeatsToSeconds(kBeats[i])), kBeats[i]);
  }
}

// Tests that a single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  constexpr float kPitch = 0.5;

  EngineImpl engine(kSampleRate);

  // Create an instrument.
  InstrumentImpl* instrument = engine.CreateInstrument({});

  // Set the note callbacks.
  float note_on_pitch = 0.0f;
  instrument->SetNoteOnCallback({
      [](float pitch, void* user_data) { *static_cast<float*>(user_data) = pitch; },
      static_cast<void*>(&note_on_pitch),
  });
  EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);

  float note_off_pitch = 0.0f;
  instrument->SetNoteOffCallback({
      [](float pitch, void* user_data) { *static_cast<float*>(user_data) = pitch; },
      static_cast<void*>(&note_off_pitch),
  });
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

  // Set a note on.
  instrument->SetNoteOn(kPitch, {});
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));
  EXPECT_FLOAT_EQ(note_on_pitch, kPitch);

  // Destroy the instrument.
  engine.DestroyInstrument(instrument);
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<float> note_off_pitches;

  {
    EngineImpl engine(kSampleRate);

    // Create instruments with note off callbacks.
    std::vector<InstrumentImpl*> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(engine.CreateInstrument({}));
      instruments[i]->SetNoteOffCallback({
          [](float pitch, void* user_data) {
            static_cast<std::vector<float>*>(user_data)->push_back(pitch);
          },
          static_cast<void*>(&note_off_pitches),
      });
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i]->SetNoteOn(static_cast<float>(i + 1), {});
      instruments[i]->SetNoteOn(static_cast<float>(-i - 1), {});
      instruments[i]->SetNoteOff(static_cast<float>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1, 2, 3));

    // Destroy instruments.
    for (int i = 0; i < 3; ++i) {
      engine.DestroyInstrument(instruments[i]);
    }
  }

  // Remaining active notes should be stopped once the engine goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0f, -2.0f, -1.0f, 1.0f, 2.0f, 3.0f));
}

// Tests that a single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  EngineImpl engine(kSampleRate);

  // Create a performer.
  PerformerImpl* performer = engine.CreatePerformer();

  // Create a task.
  TaskState task_state = TaskState::kEnd;
  double task_position = 0.0;
  std::function<void(TaskState)> process_callback = [&](TaskState state) {
    // `kUpdate` can only be called after `kBegin`, and not the other way around.
    EXPECT_TRUE(task_state != TaskState::kBegin || state == TaskState::kUpdate);
    EXPECT_TRUE(task_state != TaskState::kUpdate || state != TaskState::kBegin);
    task_state = state;
    task_position = performer->GetPosition();
  };
  auto* task = performer->CreateTask(1.0, 2.0,
                                     {
                                         [](BarelyTaskState state, void* user_data) {
                                           (*static_cast<std::function<void(TaskState)>*>(
                                               user_data))(static_cast<TaskState>(state));
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
  EXPECT_EQ(task_state, TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.0));
  engine.Update(2.5);
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.5));
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 2.5);
  EXPECT_TRUE(task->IsActive());
  EXPECT_EQ(task_state, TaskState::kUpdate);
  EXPECT_DOUBLE_EQ(task_position, 2.5);

  // Update the timestamp just past the task, which should not be active anymore.
  EXPECT_THAT(performer->GetNextDuration(), Optional(0.5));
  engine.Update(3.0);
  EXPECT_FALSE(performer->GetNextDuration().has_value());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 3.0);
  EXPECT_FALSE(task->IsActive());
  EXPECT_EQ(task_state, TaskState::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);

  // Remove the performer.
  engine.DestroyPerformer(performer);
}

// Tests that the engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  EngineImpl engine(kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);

  engine.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
}  // namespace barely
