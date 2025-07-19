#include "api/instrument.h"

#include <barelymusician.h>

#include <algorithm>
#include <array>
#include <vector>

#include "api/engine.h"
#include "gtest/gtest.h"

namespace {

constexpr int kSampleRate = 4;
constexpr float kReferenceFrequency = 1.0f;
constexpr std::array<float, kSampleRate> kSamples = {1.0f, 2.0f, 3.0f, 4.0f};

// Tests that the instrument sets a control value as expected.
TEST(InstrumentTest, SetControl) {
  BarelyEngine engine(kSampleRate, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 1.0f);

  instrument.SetControl(BarelyControlType_kGain, 0.5f);
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 0.5f);

  // Verify that the control value is clamped at the minimum value.
  instrument.SetControl(BarelyControlType_kGain, -1.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 0.0f);

  instrument.SetControl(BarelyControlType_kGain, 0.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 0.0f);

  // Verify that the control value is clamped at the maximum value.
  instrument.SetControl(BarelyControlType_kGain, 10.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 1.0f);

  instrument.SetControl(BarelyControlType_kGain, 1.0f);
  EXPECT_FLOAT_EQ(instrument.GetControl(BarelyControlType_kGain), 1.0f);
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  constexpr int kSampleCount = 5;
  constexpr float kPitch = 1.0f;
  constexpr float kGain = 0.5f;
  constexpr std::array<BarelySlice, 1> kSlices = {
      BarelySlice{kPitch, kSampleRate, kSamples.data(), kSampleRate},
  };

  BarelyEngine engine(kSampleRate, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});
  instrument.SetSampleData(kSlices);

  std::vector<float> buffer(kSampleCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, 0);
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }

  // Set a note on.
  instrument.SetNoteOn(kPitch, {{{BarelyNoteControlType_kGain, kGain}}});
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, 0);
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], (i < kSampleRate) ? kSamples[i] * kGain : 0.0f);
  }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, 0);
  for (int i = 0; i < kSampleCount; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }
}

// Tests that the instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  constexpr std::array<BarelySlice, kSampleRate> kSlices = {
      BarelySlice{0.0f, kSampleRate, kSamples.data(), 1},
      BarelySlice{1.0f, kSampleRate, kSamples.data() + 1, 1},
      BarelySlice{2.0f, kSampleRate, kSamples.data() + 2, 1},
      BarelySlice{3.0f, kSampleRate, kSamples.data() + 3, 1},
  };

  BarelyEngine engine(1, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});
  instrument.SetSampleData(kSlices);

  std::vector<float> buffer(kSampleRate);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, 0);
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }

  // Start a new note per each i in the buffer.
  for (int i = 0; i < kSampleRate; ++i) {
    instrument.SetNoteOn(static_cast<float>(i), {});
    instrument.Update(i + 1);
    instrument.SetNoteOff(static_cast<float>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, 0);
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], kSamples[i]);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument.Process(buffer, kSampleRate);
  for (int i = 0; i < kSampleRate; ++i) {
    EXPECT_FLOAT_EQ(buffer[i], 0.0f);
  }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  constexpr float kPitch = 3.3f;

  BarelyEngine engine(1, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});

  // Trigger the note on callback.
  float note_on_pitch = 0.0f;
  instrument.SetNoteOnCallback({
      [](float pitch, void* user_data) { *static_cast<float*>(user_data) = pitch; },
      static_cast<void*>(&note_on_pitch),
  });
  EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);

  instrument.SetNoteOn(kPitch, {});
  EXPECT_FLOAT_EQ(note_on_pitch, kPitch);

  // This should not trigger the callback since the note is already on.
  note_on_pitch = 0.0f;
  instrument.SetNoteOn(kPitch, {});
  EXPECT_FLOAT_EQ(note_on_pitch, 0.0f);

  // Trigger the note on callback again with another note.
  note_on_pitch = 0.0f;
  instrument.SetNoteOn(kPitch + 2.0f, {});
  EXPECT_FLOAT_EQ(note_on_pitch, kPitch + 2.0f);

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
TEST(InstrumentTest, SetAllNotesOff) {
  constexpr std::array<float, 3> kPitches = {1.0f, 2.0f, 3.0f};

  BarelyEngine engine(kSampleRate, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});
  for (const float pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const float pitch : kPitches) {
    instrument.SetNoteOn(pitch, {});
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.SetAllNotesOff();
  for (const float pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
