#include "barelymusician/internal/musician.h"

#include <array>
#include <functional>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/performer.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;
using ::testing::Optional;
using ::testing::UnorderedElementsAre;

constexpr int kFrameRate = 48000;

// Tests that the musician converts between beats and seconds as expected.
TEST(MusicianTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  Musician musician(kFrameRate);
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
  constexpr double kPitch = 0.5;
  constexpr double kIntensity = 0.75;

  Musician musician(kFrameRate);

  // Create an instrument.
  InstrumentController* instrument = musician.AddInstrument();

  // Set the note callbacks.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  NoteOnEvent::Callback note_on_callback = [&](double pitch, double intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  };
  instrument->SetNoteOnEvent(NoteOnEvent::WithCallback(), static_cast<void*>(&note_on_callback));
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  double note_off_pitch = 0.0;
  NoteOffEvent::Callback note_off_callback = [&](double pitch) { note_off_pitch = pitch; };
  instrument->SetNoteOffEvent(NoteOffEvent::WithCallback(), static_cast<void*>(&note_off_callback));
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Set a note on.
  instrument->SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));

  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  // Remove the instrument.
  musician.RemoveInstrument(instrument);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(MusicianTest, CreateDestroyMultipleInstruments) {
  std::vector<double> note_off_pitches;

  {
    Musician musician(kFrameRate);

    // Create instruments with note off callback.
    std::vector<InstrumentController*> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(musician.AddInstrument());
      NoteOffEvent::Callback note_off_callback = [&](double pitch) {
        note_off_pitches.push_back(pitch);
      };
      instruments[i]->SetNoteOffEvent(NoteOffEvent::WithCallback(),
                                      static_cast<void*>(&note_off_callback));
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i]->SetNoteOn(static_cast<double>(i + 1), 1.0);
      instruments[i]->SetNoteOn(static_cast<double>(-i - 1), 1.0);
      instruments[i]->SetNoteOff(static_cast<double>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1, 2, 3));

    // Remove instruments.
    for (int i = 0; i < 3; ++i) {
      musician.RemoveInstrument(instruments[i]);
    }
  }

  // Remaining active notes should be stopped once the musician goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0, -2.0, -1.0, 1.0, 2.0, 3.0));
}

// Tests that a single performer is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySinglePerformer) {
  Musician musician(kFrameRate);

  // Create a performer.
  Performer* performer = musician.AddPerformer(/*process_order=*/0);

  // Create a task event.
  double task_position = 0.0;
  std::function<void()> process_callback = [&]() { task_position = performer->GetPosition(); };
  auto task_event = TaskEvent{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
  };

  // Schedule a task.
  performer->ScheduleOneOffTask(task_event, 1.0, &process_callback);

  // Start the performer with a tempo of one beat per second.
  musician.SetTempo(60.0);
  EXPECT_DOUBLE_EQ(musician.GetTempo(), 60.0);

  EXPECT_FALSE(performer->IsPlaying());
  performer->Start();
  EXPECT_TRUE(performer->IsPlaying());

  // Update the timestamp just before the task, which should not be triggered.
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(1.0));
  musician.Update(1.0);
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(0.0));
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.0);
  EXPECT_DOUBLE_EQ(task_position, 0.0);

  // Update the timestamp past the task, which should be triggered now.
  EXPECT_THAT(performer->GetDurationToNextTask(), Optional(0.0));
  musician.Update(1.5);
  EXPECT_FALSE(performer->GetDurationToNextTask().has_value());
  EXPECT_DOUBLE_EQ(performer->GetPosition(), 1.5);
  EXPECT_DOUBLE_EQ(task_position, 1.0);

  // Remove the performer.
  musician.RemovePerformer(performer);
}

// Tests that the musician sets its tempo as expected.
TEST(MusicianTest, SetTempo) {
  Musician musician(kFrameRate);
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
