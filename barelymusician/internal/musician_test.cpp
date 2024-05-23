#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument.h"
#include "barelymusician/internal/performer.h"
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
      ControlDefinition{0, 0.0, -10.0, 10.0},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{0, 1.0, 0.0, 1.0},
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
      [](void** state, int32_t id, double value) {
        *reinterpret_cast<double*>(*state) = static_cast<double>(id + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {},
      [](void** /*state*/, double /*pitch*/, int32_t /*id*/, double /*value*/) {},
      [](void** state, double /*pitch*/) { *reinterpret_cast<double*>(*state) = 0.0; },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      control_definitions, note_control_definitions);
}

// Tests that the musician converts between beats and seconds as expected.
TEST(MusicianTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  Musician musician;
  musician.SetTempo(kTempo);

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(musician.GetBeatsFromSeconds(kSeconds[i]), kBeats[i]);
    EXPECT_DOUBLE_EQ(musician.GetSecondsFromBeats(kBeats[i]), kSeconds[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(musician.GetBeatsFromSeconds(musician.GetSecondsFromBeats(kBeats[i])),
                     kBeats[i]);
    EXPECT_DOUBLE_EQ(musician.GetSecondsFromBeats(musician.GetBeatsFromSeconds(kSeconds[i])),
                     kSeconds[i]);
  }
}

// Tests that a single instrument is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySingleInstrument) {
  constexpr double kPitch = -1.25;
  constexpr double kIntensity = 0.75;

  Musician musician;
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Create an instrument.
  Instrument instrument(GetTestInstrumentDefinition(), kFrameRate, musician.GetTimestamp());
  musician.AddInstrument(instrument);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
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
  instrument.SetNoteOnEvent(NoteOnEventDefinition::WithCallback(),
                            static_cast<void*>(&note_on_callback));
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  double note_off_pitch = 0.0;
  NoteOffEventDefinition::Callback note_off_callback = [&](double pitch) {
    note_off_pitch = pitch;
  };
  instrument.SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                             static_cast<void*>(&note_off_callback));
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch * kIntensity);
    }
  }

  // Remove the instrument.
  musician.RemoveInstrument(instrument);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(MusicianTest, CreateDestroyMultipleInstruments) {
  std::vector<double> note_off_pitches;

  {
    Musician musician;

    // Create instruments with note off callback.
    std::vector<std::unique_ptr<Instrument>> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(std::make_unique<Instrument>(GetTestInstrumentDefinition(), kFrameRate,
                                                         musician.GetTimestamp()));
      musician.AddInstrument(*instruments[i]);
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

    // Remove instruments.
    for (int i = 0; i < 3; ++i) {
      musician.RemoveInstrument(*instruments[i]);
    }
  }

  // Remaining active notes should be stopped once the musician goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0, -2.0, -1.0, 1.0, 2.0, 3.0));
}

// Tests that a single performer is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySinglePerformer) {
  constexpr int kProcessOrder = 0;

  Musician musician;

  // Create a performer.
  Performer performer;
  musician.AddPerformer(performer);

  // Create a task definition.
  double task_position = 0.0;
  std::function<void()> process_callback = [&]() { task_position = performer.GetPosition(); };
  auto definition = TaskDefinition{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
  };

  // Schedule a task.
  performer.ScheduleOneOffTask(definition, 1.0, kProcessOrder, &process_callback);

  // Start the performer with a tempo of one beat per second.
  musician.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 60.0);

  EXPECT_FALSE(performer.IsPlaying());
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(1.0, kProcessOrder)));
  musician.Update(1.0);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.0, kProcessOrder)));
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp past the task, which should be triggered now.
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(0.0, kProcessOrder)));
  musician.Update(1.5);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_DOUBLE_EQ(performer.GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Remove the performer.
  musician.RemovePerformer(performer);
}

// Tests that the musician sets its tempo as expected.
TEST(MusicianTest, SetTempo) {
  Musician musician;
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 120.0);

  musician.SetTempo(200.0);
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 200.0);

  musician.SetTempo(0.0);
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 0.0);

  musician.SetTempo(-100.0);
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 0.0);
}

}  // namespace
}  // namespace barely::internal
