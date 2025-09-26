#include "api/engine.h"

#include <barelymusician.h>

#include <array>
#include <functional>

#include "api/performer.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace {

using ::testing::Optional;
using ::testing::Pair;

constexpr int kSampleRate = 4;
constexpr int kChannelCount = 2;
constexpr std::array<float, kSampleRate> kSamples = {1.0f, 2.0f, 3.0f, 4.0f};

// Tests that a single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  BarelyEngine engine(kSampleRate, kSampleRate);

  // Create a performer.
  BarelyPerformer performer(engine);

  // Create a task.
  barely::TaskEventType task_event_type = barely::TaskEventType::kEnd;
  double task_position = 0.0;
  std::function<void(barely::TaskEventType)> process_callback = [&](barely::TaskEventType type) {
    task_event_type = type;
    task_position = performer.GetPosition();
  };
  const BarelyTask task(performer, 1.0, 2.0, 0,
                        {
                            [](BarelyTaskEventType type, void* user_data) {
                              (*static_cast<std::function<void(barely::TaskEventType)>*>(
                                  user_data))(static_cast<barely::TaskEventType>(type));
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
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp inside the task, which should be triggered now.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.0, 0)));
  engine.Update(2.5);
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 2.5);
  EXPECT_TRUE(task.IsActive());
  EXPECT_EQ(task_event_type, barely::TaskEventType::kBegin);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Update the timestamp just past the task, which should not be active anymore.
  EXPECT_THAT(performer.GetNextTaskKey(), Optional(Pair(0.5, 0)));
  engine.Update(3.0);
  EXPECT_FALSE(performer.GetNextTaskKey().has_value());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 3.0);
  EXPECT_FALSE(task.IsActive());
  EXPECT_EQ(task_event_type, barely::TaskEventType::kEnd);
  EXPECT_DOUBLE_EQ(task_position, 3.0);
}

// Tests that an instrument plays a single note as expected.
TEST(EngineTest, PlaySingleNote) {
  constexpr int kFrameCount = 5;
  constexpr float kPitch = 1.0f;
  constexpr float kGain = 0.5f;
  constexpr std::array<BarelySlice, 1> kSlices = {
      BarelySlice{kPitch, kSampleRate, kSamples.data(), kSampleRate},
  };

  BarelyEngine engine(kSampleRate, kFrameCount);
  BarelyInstrument instrument(engine, {});
  instrument.SetSampleData(kSlices);

  std::array<float, kChannelCount * kFrameCount> samples;

  // Control is set to its default value.
  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kFrameCount, 0);
  for (int i = 0; i < kChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }

  // Set a note on.
  instrument.SetNoteOn(kPitch, {{{BarelyNoteControlType_kGain, kGain}}});
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kFrameCount, 0);
  for (int i = 0; i < kChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], (i / kChannelCount < kSampleRate)
                                    ? 0.5f * kSamples[i / kChannelCount] * kGain
                                    : 0.0f);
  }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kFrameCount, 0);
  for (int i = 0; i < kChannelCount * kFrameCount; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }
}

// Tests that an instrument plays multiple notes as expected.
TEST(EngineTest, PlayMultipleNotes) {
  constexpr std::array<BarelySlice, kSampleRate> kSlices = {
      BarelySlice{0.0f, kSampleRate, kSamples.data(), 1},
      BarelySlice{1.0f, kSampleRate, kSamples.data() + 1, 1},
      BarelySlice{2.0f, kSampleRate, kSamples.data() + 2, 1},
      BarelySlice{3.0f, kSampleRate, kSamples.data() + 3, 1},
  };

  BarelyEngine engine(1, kSampleRate);
  BarelyInstrument instrument(engine, {});
  instrument.SetSampleData(kSlices);

  std::array<float, kChannelCount * kSampleRate> samples;

  // Control is set to its default value.
  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kSampleRate, 0);
  for (int i = 0; i < kChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }

  // Start a new note per each i in the samples.
  for (int i = 0; i < kSampleRate; ++i) {
    instrument.SetNoteOn(static_cast<float>(i), {});
    engine.Update(i + 1);
    instrument.SetNoteOff(static_cast<float>(i));
  }

  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kSampleRate, 0);
  for (int i = 0; i < kChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.5f * kSamples[i / kChannelCount]);
  }

  samples.fill(0.0f);
  engine.Process(samples.data(), kChannelCount, kSampleRate, 0);
  for (int i = 0; i < kChannelCount * kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(samples[i], 0.0f);
  }
}

// Tests that the engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  BarelyEngine engine(kSampleRate, kSampleRate);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);

  engine.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
