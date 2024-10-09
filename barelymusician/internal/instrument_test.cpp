#include "barelymusician/internal/instrument.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::IsNull;
using ::testing::Pointee;
using ::testing::Property;

constexpr int kFrameRate = 8000;
constexpr int kChannelCount = 1;
constexpr int kFrameCount = 4;

// Tests that the instrument returns a control value as expected.
TEST(InstrumentTest, GetControl) {
  ASSERT_EQ(static_cast<int>(InstrumentControl::kGain), 0);

  Instrument instrument(kFrameRate, 0);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 1.0)));

  instrument.GetControl(0)->SetValue(0.25);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 0.25)));

  instrument.GetControl(0)->ResetValue();
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 1.0)));

  instrument.GetControl(0)->SetValue(-2.0);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 0.0)));

  instrument.ResetAllControls();
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 1.0)));

  // Control does not exist.
  EXPECT_THAT(instrument.GetControl(-1), IsNull());
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  constexpr double kPitch = 1.0;
  constexpr double kIntensity = 0.5;
  constexpr int64_t kUpdateFrame = 20;

  Instrument instrument(kFrameRate, kUpdateFrame);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // TODO(#139): Reenable the sample checks once the reference frequency is added.

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
  //   }
  // }

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch * kIntensity);
  //   }
  // }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
  //   }
  // }
}

// Tests that the instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  constexpr double kIntensity = 1.0;

  Instrument instrument(1, 0);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // TODO(#139): Reenable the sample checks once the reference frequency is added.

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
  //   }
  // }

  // Start a new note per each frame in the buffer.
  for (int i = 0; i < kFrameCount; ++i) {
    instrument.SetNoteOn(i, kIntensity);
    instrument.Update(i + 1);
    instrument.SetNoteOff(i);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
  //                      static_cast<double>(frame) * kIntensity);
  //   }
  // }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kFrameCount));
  // for (int frame = 0; frame < kFrameCount; ++frame) {
  //   for (int channel = 0; channel < kChannelCount; ++channel) {
  //     EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
  //   }
  // }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  constexpr double kPitch = 3.3;
  constexpr double kIntensity = 0.25;

  Instrument instrument(1, 0);

  // Trigger the note on callback.
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

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  // This should not trigger the callback since the note is already on.
  note_on_pitch = 0.0;
  note_on_intensity = 0.0;
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, 0.0);

  // Trigger the note on callback again with another note.
  note_on_pitch = 0.0;
  note_on_intensity = 0.0;
  instrument.SetNoteOn(kPitch + 2, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch + 2.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

  // Trigger the note off callback.
  double note_off_pitch = 0.0;
  NoteOffEventDefinition::Callback note_off_callback = [&](double pitch) {
    note_off_pitch = pitch;
  };
  instrument.SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                             static_cast<void*>(&note_off_callback));
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  instrument.SetNoteOff(kPitch);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);

  // This should not trigger the callback since the note is already off.
  note_off_pitch = 0.0;
  instrument.SetNoteOff(kPitch);
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Finally, trigger the note off callback with the remaining note.
  instrument.SetAllNotesOff();
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch + 2.0);
}

// Tests that the instrument stops all notes as expected.
TEST(InstrumentTest, SetAllNotesOff) {
  constexpr std::array<double, 3> kPitches = {1.0, 2.0, 3.0};
  constexpr double kIntensity = 1.0;

  Instrument instrument(kFrameRate, 0);
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const double pitch : kPitches) {
    instrument.SetNoteOn(pitch, kIntensity);
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.SetAllNotesOff();
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely::internal
