#include "engine/musician.h"

#include <array>
#include <functional>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "engine/instrument.h"
#include "engine/performer.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::ElementsAre;
using ::testing::Optional;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

constexpr int kSampleRate = 48000;

// Tests that the musician converts between beats and seconds as expected.
TEST(MusicianTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  Musician musician(kSampleRate);
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
  constexpr float kPitch = 0.5;
  constexpr float kIntensity = 0.75;

  Musician musician(kSampleRate);

  // Create an instrument.
  Instrument* instrument = musician.CreateInstrument();

  // Set the note callbacks.
  std::pair<float, float> note_on_state = {0.0f, 0.0f};
  instrument->SetNoteOnCallback(
      [](float pitch, float intensity, void* user_data) {
        auto& note_on_state = *static_cast<std::pair<float, float>*>(user_data);
        note_on_state.first = pitch;
        note_on_state.second = intensity;
      },
      static_cast<void*>(&note_on_state));
  EXPECT_THAT(note_on_state, Pair(0.0f, 0.0f));

  float note_off_pitch = 0.0f;
  instrument->SetNoteOffCallback(
      [](float pitch, void* user_data) { *static_cast<float*>(user_data) = pitch; },
      static_cast<void*>(&note_off_pitch));
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

  // Set a note on.
  instrument->SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument->IsNoteOn(kPitch));
  EXPECT_THAT(note_on_state, Pair(kPitch, kIntensity));

  // Destroy the instrument.
  musician.DestroyInstrument(instrument);
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch);
}

// Tests that multiple instruments are created and destroyed as expected.
TEST(MusicianTest, CreateDestroyMultipleInstruments) {
  std::vector<float> note_off_pitches;

  {
    Musician musician(kSampleRate);

    // Create instruments with note off callbacks.
    std::vector<Instrument*> instruments;
    for (int i = 0; i < 3; ++i) {
      instruments.push_back(musician.CreateInstrument());
      instruments[i]->SetNoteOffCallback(
          [](float pitch, void* user_data) {
            static_cast<std::vector<float>*>(user_data)->push_back(pitch);
          },
          static_cast<void*>(&note_off_pitches));
    }

    // Start multiple notes, then immediately stop some of them.
    for (int i = 0; i < 3; ++i) {
      instruments[i]->SetNoteOn(static_cast<float>(i + 1), 1.0f);
      instruments[i]->SetNoteOn(static_cast<float>(-i - 1), 1.0f);
      instruments[i]->SetNoteOff(static_cast<float>(i + 1));
    }
    EXPECT_THAT(note_off_pitches, ElementsAre(1, 2, 3));

    // Destroy instruments.
    for (int i = 0; i < 3; ++i) {
      musician.DestroyInstrument(instruments[i]);
    }
  }

  // Remaining active notes should be stopped once the musician goes out of scope.
  EXPECT_THAT(note_off_pitches, UnorderedElementsAre(-3.0f, -2.0f, -1.0f, 1.0f, 2.0f, 3.0f));
}

// Tests that a single performer is created and destroyed as expected.
TEST(MusicianTest, CreateDestroySinglePerformer) {
  Musician musician(kSampleRate);

  // Create a performer.
  Performer* performer = musician.CreatePerformer(/*process_order=*/0);

  // Create a task event.
  double task_position = 0.0;
  std::function<void()> process_callback = [&]() { task_position = performer->GetPosition(); };
  auto task_event = TaskEvent{
      [](void** state, void* user_data) { *state = user_data; },
      [](void** /*state*/) {},
      [](void** state) { (*static_cast<std::function<void()>*>(*state))(); },
      &process_callback,
  };

  // Schedule a task.
  performer->CreateTask(task_event, 1.0);

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
  musician.DestroyPerformer(performer);
}

// Tests that the musician sets its tempo as expected.
TEST(MusicianTest, SetTempo) {
  Musician musician(kSampleRate);
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
