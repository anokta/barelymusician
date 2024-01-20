#include "barelymusician/internal/musician.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
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
      ControlDefinition{0, -10, 10},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{1, 0, 1},
  };
  return InstrumentDefinition(
      [](void** state, int32_t /*frame_rate*/) { *state = reinterpret_cast<void*>(new float{0}); },
      [](void** state) { delete static_cast<float*>(*state); },
      [](void** state, float* output_samples, int32_t output_channel_count,
         int32_t output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<float*>(*state));
      },
      [](void** state, int32_t index, BarelyRational value, BarelyRational /*slope_per_frame*/) {
        *reinterpret_cast<float*>(*state) = static_cast<float>((index + 1) * Rational(value));
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {},
      [](void** /*state*/, BarelyRational /*pitch*/, int32_t /*index*/, BarelyRational /*value*/,
         BarelyRational /*slope_per_frame*/) {},
      [](void** state, BarelyRational /*pitch*/) { *reinterpret_cast<float*>(*state) = 0; },
      [](void** state, BarelyRational pitch, BarelyRational intensity) {
        *reinterpret_cast<float*>(*state) =
            static_cast<float>(Rational(pitch) * Rational(intensity));
      },
      control_definitions, note_control_definitions);
}

// Tests that a single instrument is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySingleInstrument) {
  constexpr Rational kPitch = Rational(-5, 4);
  constexpr Rational kIntensity = Rational(3, 4);

  Musician musician(kFrameRate);
  std::vector<float> buffer(kChannelCount * kFrameCount);

  // Create an instrument.
  Instrument instrument(GetTestInstrumentDefinition(), kFrameRate, musician.GetTempo(),
                        musician.GetTimestamp());
  musician.AddInstrument(instrument);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kChannelCount * frame + channel], 0);
    }
  }

  // Set the note callbacks.
  Rational note_on_pitch = 0;
  Rational note_on_intensity = 0;
  NoteOnEventDefinition::Callback note_on_callback = [&](Rational pitch, Rational intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  };
  instrument.SetNoteOnEvent(NoteOnEventDefinition::WithCallback(),
                            static_cast<void*>(&note_on_callback));
  EXPECT_EQ(note_on_pitch, 0);
  EXPECT_EQ(note_on_intensity, 0);

  Rational note_off_pitch = 0;
  NoteOffEventDefinition::Callback note_off_callback = [&](Rational pitch) {
    note_off_pitch = pitch;
  };
  instrument.SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                             static_cast<void*>(&note_off_callback));
  EXPECT_EQ(note_off_pitch, 0);

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  EXPECT_EQ(note_on_pitch, kPitch);
  EXPECT_EQ(note_on_intensity, kIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kChannelCount * frame + channel],
                      static_cast<float>(kPitch * kIntensity));
    }
  }

  // Remove the instrument.
  musician.RemoveInstrument(instrument);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(MusicianTest, CreateDestroyMultipleInstruments) {
  std::vector<Rational> note_off_pitches;

  {
    Musician musician(kFrameRate);

    // Create instruments with note off callback.
    std::vector<std::unique_ptr<Instrument>> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(std::make_unique<Instrument>(
          GetTestInstrumentDefinition(), kFrameRate, musician.GetTempo(), musician.GetTimestamp()));
      musician.AddInstrument(*instruments[i]);
      NoteOffEventDefinition::Callback note_off_callback = [&](Rational pitch) {
        note_off_pitches.push_back(pitch);
      };
      instruments[i]->SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                                      static_cast<void*>(&note_off_callback));
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i]->SetNoteOn(i + 1);
      instruments[i]->SetNoteOn(-i - 1);
      instruments[i]->SetNoteOff(i + 1);
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1, 2, 3));

    // Remove instruments.
    for (int i = 0; i < 3; ++i) {
      musician.RemoveInstrument(*instruments[i]);
    }
  }

  // Remaining active notes should be stopped once the musician goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3, -2, -1, 1, 2, 3));
}

// Tests that a single performer is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySinglePerformer) {
  constexpr int kProcessOrder = 0;

  Musician musician(kFrameRate);

  // Create a performer.
  Performer performer;
  musician.AddPerformer(performer);

  // Create a task definition.
  Rational task_position = 0;
  std::function<void()> process_callback = [&]() { task_position = performer.GetPosition(); };
  auto definition = TaskDefinition{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
  };

  // Schedule a task.
  performer.ScheduleOneOffTask(definition, 1, kProcessOrder, &process_callback);

  // Start the performer with a tempo of one beat per second.
  musician.SetTempo(60);
  EXPECT_EQ(musician.GetTempo(), 60);

  EXPECT_FALSE(performer.IsPlaying());
  performer.Start();
  EXPECT_TRUE(performer.IsPlaying());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(1), kProcessOrder)));
  musician.Update(kFrameRate);
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(0), kProcessOrder)));
  EXPECT_EQ(performer.GetPosition(), 1);
  EXPECT_EQ(task_position, 0);

  // Update the timestamp past the task, which should be triggered now.
  EXPECT_THAT(performer.GetDurationToNextTask(), Optional(Pair(Rational(0), kProcessOrder)));
  musician.Update(kFrameRate * 3 / 2);
  EXPECT_FALSE(performer.GetDurationToNextTask().has_value());
  EXPECT_EQ(performer.GetPosition(), Rational(3, 2));
  EXPECT_EQ(task_position, 1);

  // Remove the performer.
  musician.RemovePerformer(performer);
}

// Tests that the musician sets its tempo as expected.
TEST(MusicianTest, SetTempo) {
  Musician musician(kFrameRate);
  EXPECT_EQ(musician.GetTempo(), 120);

  musician.SetTempo(200);
  EXPECT_EQ(musician.GetTempo(), 200);

  musician.SetTempo(0);
  EXPECT_EQ(musician.GetTempo(), 0);

  musician.SetTempo(-100);
  EXPECT_EQ(musician.GetTempo(), 0);
}

}  // namespace
}  // namespace barely::internal
