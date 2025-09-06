#include "api/instrument.h"

#include <barelymusician.h>

#include <array>
#include <utility>

#include "api/engine.h"
#include "gtest/gtest.h"

namespace {

constexpr int kSampleRate = 4;
constexpr int kChannelCount = 2;
constexpr float kReferenceFrequency = 1.0f;

// Tests that the instrument sets a control value as expected.
TEST(InstrumentTest, SetControl) {
  BarelyEngine engine(kSampleRate, kChannelCount, kSampleRate, kReferenceFrequency);
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

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  constexpr float kPitch = 3.3f;

  BarelyEngine engine(1, kChannelCount, 1, kReferenceFrequency);
  BarelyInstrument instrument(engine, {});

  // Trigger the note on callback.
  std::pair<float, float> note_pitch = {0.0f, 0.0f};
  instrument.SetNoteEventCallback({
      [](BarelyNoteEventType type, float pitch, void* user_data) {
        (type == BarelyNoteEventType_kBegin
             ? static_cast<std::pair<float, float>*>(user_data)->first
             : static_cast<std::pair<float, float>*>(user_data)->second) = pitch;
      },
      static_cast<void*>(&note_pitch),
  });
  EXPECT_FLOAT_EQ(note_pitch.first, 0.0f);

  instrument.SetNoteOn(kPitch, {});
  EXPECT_FLOAT_EQ(note_pitch.first, kPitch);

  // This should not trigger the callback since the note is already on.
  note_pitch.first = 0.0f;
  instrument.SetNoteOn(kPitch, {});
  EXPECT_FLOAT_EQ(note_pitch.first, 0.0f);

  // Trigger the note on callback again with another note.
  note_pitch.first = 0.0f;
  instrument.SetNoteOn(kPitch + 2.0f, {});
  EXPECT_FLOAT_EQ(note_pitch.first, kPitch + 2.0f);

  // Trigger the note off callback.
  instrument.SetNoteOff(kPitch);
  EXPECT_FLOAT_EQ(note_pitch.second, kPitch);

  // This should not trigger the callback since the note is already off.
  note_pitch.second = 0.0;
  instrument.SetNoteOff(kPitch);
  EXPECT_FLOAT_EQ(note_pitch.second, 0.0f);

  // Finally, trigger the note off callback with the remaining note.
  instrument.SetAllNotesOff();
  EXPECT_FLOAT_EQ(note_pitch.second, kPitch + 2.0f);
}

// Tests that the instrument stops all notes as expected.
TEST(InstrumentTest, SetAllNotesOff) {
  constexpr std::array<float, 3> kPitches = {1.0f, 2.0f, 3.0f};

  BarelyEngine engine(kSampleRate, kChannelCount, kSampleRate, kReferenceFrequency);
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
