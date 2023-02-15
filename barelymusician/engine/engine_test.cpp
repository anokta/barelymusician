#include "barelymusician/engine/engine.h"

#include <algorithm>
#include <array>
#include <vector>

#include "barelymusician/engine/control.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/status.h"
#include "barelymusician/engine/task.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

constexpr int kFrameRate = 48000;
constexpr int kChannelCount = 2;
constexpr int kFrameCount = 8;

// Returns test instrument definition that produces constant output that is set.
InstrumentDefinition GetTestInstrumentDefinition() {
  static const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{0.0, -10.0, 10.0},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
      [](void** state, int /*frame_rate*/) {
        *state = reinterpret_cast<void*>(new double{0.0});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int output_channel_count,
         int output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int index, double value, double /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) =
            static_cast<double>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int /*size*/) {},
      [](void** /*state*/, double /*pitch*/, int /*index*/, double /*value*/,
         double /*slope_per_frame*/) {},
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      control_definitions, note_control_definitions);
}

// Tests that single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  const double kPitch = -1.25;
  const double kIntensity = 0.75;

  Engine engine;
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Create an instrument.
  const auto instrument_id_or =
      engine.CreateInstrument(GetTestInstrumentDefinition(), kFrameRate);
  ASSERT_TRUE(instrument_id_or.IsOk());
  const Id instrument_id = *instrument_id_or;

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_EQ(engine.ProcessInstrument(instrument_id, buffer.data(),
                                     kChannelCount, kFrameCount, 0.0),
            Status::Ok());
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Get the instrument.
  const auto instrument_or = engine.GetInstrument(instrument_id);
  ASSERT_TRUE(instrument_or.IsOk());
  auto& instrument = instrument_or->get();

  // Set note callbacks.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  instrument.SetNoteOnEventCallback([&](double pitch, double intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  });
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  double note_off_pitch = 0.0;
  instrument.SetNoteOffEventCallback(
      [&](double pitch) { note_off_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_EQ(engine.ProcessInstrument(instrument_id, buffer.data(),
                                     kChannelCount, kFrameCount, 0.0),
            Status::Ok());
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       kPitch * kIntensity);
    }
  }

  // Update timestamp.
  engine.Update(5.0);

  // Destroy the instrument, which should also trigger the note off callback.
  note_off_pitch = 0.0;
  EXPECT_EQ(engine.DestroyInstrument(instrument_id), Status::Ok());
  ASSERT_FALSE(engine.GetInstrument(instrument_id).IsOk());
  EXPECT_EQ(engine.GetInstrument(instrument_id).GetErrorStatus(),
            Status::NotFound());
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_EQ(engine.ProcessInstrument(instrument_id, buffer.data(),
                                     kChannelCount, kFrameCount, 0.0),
            Status::NotFound());
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<double> note_off_pitches;

  {
    Engine engine;

    // Create instruments with note off callback.
    std::array<Id, 3> instrument_ids;
    for (int i = 0; i < 3; ++i) {
      const auto instrument_id_or =
          engine.CreateInstrument(GetTestInstrumentDefinition(), kFrameRate);
      ASSERT_TRUE(instrument_id_or.IsOk());
      instrument_ids[i] = *instrument_id_or;
      const auto instrument_or = engine.GetInstrument(instrument_ids[i]);
      ASSERT_TRUE(instrument_or.IsOk());
      instrument_or->get().SetNoteOffEventCallback(
          [&](double pitch) { note_off_pitches.push_back(pitch); });
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      const auto instrument_or = engine.GetInstrument(instrument_ids[i]);
      ASSERT_TRUE(instrument_or.IsOk());
      auto& instrument = instrument_or->get();
      instrument.SetNoteOn(static_cast<double>(i + 1), 1.0);
      instrument.SetNoteOn(static_cast<double>(-i - 1), 1.0);
      instrument.SetNoteOff(static_cast<double>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1.0, 2.0, 3.0));
  }

  // Remaining active notes should be stopped once the engine goes out of scope.
  EXPECT_THAT(note_off_pitches,
              UnorderedElementsAre(-3.0, -2.0, -1.0, 1.0, 2.0, 3.0));
}

// Tests that single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  const int kProcessOrder = 0;

  Engine engine;

  // Create a performer.
  const auto performer_id_or = engine.CreatePerformer();
  ASSERT_TRUE(performer_id_or.IsOk());
  const Id performer_id = *performer_id_or;

  const auto performer_or = engine.GetPerformer(performer_id);
  ASSERT_TRUE(performer_or.IsOk());
  auto& performer = performer_or->get();

  // Create a task definition.
  double task_position = 0.0;
  std::function<void()> process_callback = [&]() {
    task_position = performer.GetPosition();
  };
  auto definition = TaskDefinition{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
  };

  // Create a task.
  ASSERT_TRUE(engine
                  .CreatePerformerTask(performer_id, definition,
                                       /*is_one_off=*/false, 1.0, kProcessOrder,
                                       &process_callback)
                  .IsOk());

  // Start the performer with a tempo of one beat per second.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer.IsPlaying());
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer.GetDurationToNextTask(),
              Optional(Pair(1.0, kProcessOrder)));
  engine.Update(1.0);
  EXPECT_THAT(performer.GetDurationToNextTask(),
              Optional(Pair(0.0, kProcessOrder)));
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp past the task, which should be triggered now.
  EXPECT_THAT(performer.GetDurationToNextTask(),
              Optional(Pair(0.0, kProcessOrder)));
  engine.Update(1.5);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Destroy the performer.
  EXPECT_EQ(engine.DestroyPerformer(performer_id), Status::Ok());
  EXPECT_FALSE(engine.GetPerformer(performer_id).IsOk());
}

// TODO(#108): Add `CreateDestroyMultiplePerformers` using differing priorities.

// Tests that engine sets its tempo as expected.
TEST(EngineTest, SetTempo) {
  Engine engine;
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 120.0);

  engine.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 200.0);

  engine.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);

  engine.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 0.0);
}

}  // namespace
}  // namespace barely::internal
