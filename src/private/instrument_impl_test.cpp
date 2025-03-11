#include "private/instrument_impl.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "dsp/decibels.h"
#include "dsp/sample_data.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Pair;

constexpr int kSampleRate = 4;
constexpr float kReferenceFrequency = 1.0f;
constexpr std::array<float, kSampleRate> kSamples = {1.0f, 2.0f, 3.0f, 4.0f};

// Tests that the instrument sets a control value as expected.
TEST(InstrumentImplTest, SetControl) {
  InstrumentImpl instrument(kSampleRate, kReferenceFrequency, 0);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), 0.0f);

  instrument.SetControl(ControlType::kGain, -12.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), -12.0f);

  // Verify that the control value is clamped at the minimum value.
  instrument.SetControl(ControlType::kGain, -100.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), kMinDecibels);

  instrument.SetControl(ControlType::kGain, 0.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), 0.0f);

  // Verify that the control value is clamped at the maximum value.
  instrument.SetControl(ControlType::kGain, 10.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), 0.0f);

  instrument.SetControl(ControlType::kGain, 0.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(ControlType::kGain), 0.0f);
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentImplTest, PlaySingleNote) {
  constexpr int kSampleCount = 5;
  constexpr float kPitch = 1.0f;
  constexpr float kIntensity = 0.5f;
  constexpr int64_t kUpdateSample = 20;
  constexpr std::array<Slice, 1> kSlices = {Slice(kPitch, kSampleRate, kSamples)};

  InstrumentImpl instrument(kSampleRate, kReferenceFrequency, kUpdateSample);
  instrument.SetSampleData(SampleData(kSlices));

  std::vector<float> buffer(kSampleCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, kUpdateSample));
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, kUpdateSample));
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], (i < kSampleRate) ? kSamples[i] * kIntensity : 0.0f);
  }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, kUpdateSample));
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }
}

// Tests that the instrument plays multiple notes as expected.
TEST(InstrumentImplTest, PlayMultipleNotes) {
  constexpr std::array<Slice, kSampleRate> kSlices = {
      Slice(0.0f, kSampleRate, {kSamples.data(), kSamples.data() + 1}),
      Slice(1.0f, kSampleRate, {kSamples.data() + 1, kSamples.data() + 2}),
      Slice(2.0f, kSampleRate, {kSamples.data() + 2, kSamples.data() + 3}),
      Slice(3.0f, kSampleRate, {kSamples.data() + 3, kSamples.data() + 4}),
  };

  InstrumentImpl instrument(1, kReferenceFrequency, 0);
  instrument.SetSampleData(SampleData(kSlices));

  std::vector<float> buffer(kSampleRate);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, 0));
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }

  // Start a new note per each i in the buffer.
  for (int i = 0; i < kSampleRate; ++i) {
    instrument.SetNoteOn(static_cast<float>(i), 1.0f);
    instrument.Update(i + 1);
    instrument.SetNoteOff(static_cast<float>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, 0));
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], kSamples[i]);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  EXPECT_TRUE(instrument.Process(buffer, kSampleRate));
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentImplTest, SetNoteCallbacks) {
  constexpr float kPitch = 3.3f;
  constexpr float kIntensity = 0.25f;

  InstrumentImpl instrument(1, kReferenceFrequency, 0);

  // Trigger the note on callback.
  std::pair<float, float> note_on_state = {0.0f, 0.0f};
  instrument.SetNoteOnCallback({
      [](float pitch, float intensity, void* user_data) {
        auto& note_on_state = *static_cast<std::pair<float, float>*>(user_data);
        note_on_state.first = pitch;
        note_on_state.second = intensity;
      },
      static_cast<void*>(&note_on_state),
  });
  EXPECT_THAT(note_on_state, Pair(0.0f, 0.0f));

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_THAT(note_on_state, Pair(kPitch, kIntensity));

  // This should not trigger the callback since the note is already on.
  note_on_state = {0.0f, 0.0f};
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_THAT(note_on_state, Pair(0.0f, 0.0f));

  // Trigger the note on callback again with another note.
  note_on_state = {0.0f, 0.0f};
  instrument.SetNoteOn(kPitch + 2, kIntensity);
  EXPECT_THAT(note_on_state, Pair(kPitch + 2.0f, kIntensity));

  // Trigger the note off callback.
  float note_off_pitch = 0.0f;
  instrument.SetNoteOffCallback({
      [](float pitch, void* user_data) { *static_cast<float*>(user_data) = pitch; },
      static_cast<void*>(&note_off_pitch),
  });
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

  instrument.SetNoteOff(kPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch);

  // This should not trigger the callback since the note is already off.
  note_off_pitch = 0.0;
  instrument.SetNoteOff(kPitch);
  EXPECT_FLOAT_EQ(note_off_pitch, 0.0f);

  // Finally, trigger the note off callback with the remaining note.
  instrument.SetAllNotesOff();
  EXPECT_FLOAT_EQ(note_off_pitch, kPitch + 2.0f);
}

// Tests that the instrument stops all notes as expected.
TEST(InstrumentImplTest, SetAllNotesOff) {
  constexpr std::array<float, 3> kPitches = {1.0f, 2.0f, 3.0f};
  constexpr float kIntensity = 1.0f;

  InstrumentImpl instrument(kSampleRate, kReferenceFrequency, 0);
  for (const float pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const float pitch : kPitches) {
    instrument.SetNoteOn(pitch, kIntensity);
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.SetAllNotesOff();
  for (const float pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely
