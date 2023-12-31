#include "barelymusician/internal/engine.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

#include <algorithm>
#include <functional>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/observable.h"
#include "gmock/gmock-matchers.h"
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

// Returns a test instrument definition that produces constant output per note.
InstrumentDefinition GetTestInstrumentDefinition() {
  static const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{0.0, -10.0, 10.0},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
      [](void** state, int32_t /*frame_rate*/) {
        *state = reinterpret_cast<void*>(new double{0.0});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int32_t output_channel_count,
         int32_t output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int32_t index, double value, double /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) = static_cast<double>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {},
      [](void** /*state*/, double /*pitch*/, int32_t /*index*/, double /*value*/,
         double /*slope_per_frame*/) {},
      [](void** state, double /*pitch*/) { *reinterpret_cast<double*>(*state) = 0.0; },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      control_definitions, note_control_definitions);
}

// Tests that a single instrument is created and destroyed as expected.
TEST(EngineTest, CreateDestroySingleInstrument) {
  constexpr double kPitch = -1.25;
  constexpr double kIntensity = 0.75;

  Engine engine;
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Create an instrument.
  const auto instrument = engine.CreateInstrument(GetTestInstrumentDefinition(), kFrameRate);
  ASSERT_NE(instrument.get(), nullptr);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument->Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Set the note callbacks.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  NoteOnEventDefinition::Callback note_on_callback = [&](double pitch, double intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  };
  instrument->SetNoteOnEvent(NoteOnEventDefinition::WithCallback(),
                             static_cast<void*>(&note_on_callback));
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  double note_off_pitch = 0.0;
  NoteOffEventDefinition::Callback note_off_callback = [&](double pitch) {
    note_off_pitch = pitch;
  };
  instrument->SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                              static_cast<void*>(&note_off_callback));
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Set a note on.
  instrument->SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument->Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch * kIntensity);
    }
  }

  // Destroy the instrument.
  engine.DestroyInstrument(instrument);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(EngineTest, CreateDestroyMultipleInstruments) {
  std::vector<double> note_off_pitches;

  {
    Engine engine;

    // Create instruments with note off callback.
    std::vector<Observer<Instrument>> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(engine.CreateInstrument(GetTestInstrumentDefinition(), kFrameRate));
      ASSERT_NE(instruments[i].get(), nullptr);
      NoteOffEventDefinition::Callback note_off_callback = [&](double pitch) {
        note_off_pitches.push_back(pitch);
      };
      instruments[i]->SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                                      static_cast<void*>(&note_off_callback));
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i]->SetNoteOn(static_cast<double>(i + 1), 1.0);
      instruments[i]->SetNoteOn(static_cast<double>(-i - 1), 1.0);
      instruments[i]->SetNoteOff(static_cast<double>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1.0, 2.0, 3.0));
  }

  // Remaining active notes should be stopped once the engine goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0, -2.0, -1.0, 1.0, 2.0, 3.0));
}

// Tests that a single performer is created and destroyed as expected.
TEST(EngineTest, CreateDestroySinglePerformer) {
  constexpr int kProcessOrder = 0;

  Engine engine;

  // Create a performer.
  const auto performer = engine.CreatePerformer();

  // Create a task definition.
  double task_position = 0.0;
  std::function<void()> process_callback = [&]() { task_position = performer->GetPosition(); };
  auto definition = TaskDefinition{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
  };

  // Create a task.
  ASSERT_TRUE(engine.CreatePerformerTask(performer.get(), definition,
                                         /*is_one_off=*/false, 1.0, kProcessOrder,
                                         &process_callback));

  // Start the performer with a tempo of one beat per second.
  engine.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(engine.GetTempo(), 60.0);

  EXPECT_FALSE(performer->IsPlaying());
  performer->Start();
  EXPECT_TRUE(performer->IsPlaying());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(Pair(1.0, kProcessOrder)));
  engine.Update(1.0);
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(Pair(0.0, kProcessOrder)));
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp past the task, which should be triggered now.
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(Pair(0.0, kProcessOrder)));
  engine.Update(1.5);
  EXPECT_FALSE(performer->GetDurationToNextTask().has_value());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Destroy the performer.
  engine.DestroyPerformer(performer);
}

// Tests that the engine sets its tempo as expected.
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
