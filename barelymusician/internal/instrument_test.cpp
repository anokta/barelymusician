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

constexpr double kTempo = 60.0;

// Returns a test instrument definition that produces constant output per note.
InstrumentDefinition GetTestDefinition() {
  static const std::array<ControlDefinition, 1> control_definitions = {
      ControlDefinition{15.0, 10.0, 20.0},
  };
  static const std::array<ControlDefinition, 1> note_control_definitions = {
      ControlDefinition{1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
      [](void** state, int /*frame_rate*/) { *state = static_cast<void*>(new double{0.0}); },
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

// Tests that the instrument returns a control value as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0.0);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 15.0)));

  EXPECT_TRUE(instrument.SetControl(0, 20.0, 0.0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 20.0)));

  EXPECT_TRUE(instrument.ResetControl(0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 15.0)));

  EXPECT_TRUE(instrument.SetControl(0, 50.0, 0.0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 20.0)));

  instrument.ResetAllControls();
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 15.0)));

  // Control does not exist.
  EXPECT_THAT(instrument.GetControl(1), IsNull());
  EXPECT_FALSE(instrument.SetControl(1, 2.0, 0.0));
  EXPECT_FALSE(instrument.ResetControl(1));
}

// Tests that the instrument returns a note control value as expected.
TEST(InstrumentTest, GetNoteControl) {
  constexpr double kPitch = 1.0;
  constexpr double kIntensity = 1.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0.0);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));
  EXPECT_FALSE(instrument.GetNoteControl(kPitch, 0));

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), Pointee(Property(&Control::GetValue, 1.0)));

  EXPECT_TRUE(instrument.SetNoteControl(kPitch, 0, 0.25, 0.0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), Pointee(Property(&Control::GetValue, 0.25)));

  EXPECT_TRUE(instrument.ResetNoteControl(kPitch, 0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), Pointee(Property(&Control::GetValue, 1.0)));

  EXPECT_TRUE(instrument.SetNoteControl(kPitch, 0, -10.0, 0.0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), Pointee(Property(&Control::GetValue, 0.0)));

  instrument.ResetAllNoteControls(kPitch);
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), Pointee(Property(&Control::GetValue, 1.0)));

  // Note control does not exist.
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 1), IsNull());
  EXPECT_FALSE(instrument.SetNoteControl(kPitch, 1, 0.25, 0.0));
  EXPECT_FALSE(instrument.ResetNoteControl(kPitch, 1));

  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), IsNull());
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  constexpr double kPitch = 32.0;
  constexpr double kIntensity = 0.5;
  constexpr double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, kTimestamp);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch * kIntensity);
    }
  }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that the instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  constexpr double kIntensity = 1.0;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0.0);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Start a new note per each frame in the buffer.
  for (int i = 0; i < kFrameCount; ++i) {
    instrument.SetNoteOn(static_cast<double>(i), kIntensity);
    instrument.Update(static_cast<double>(i + 1));
    instrument.SetNoteOff(static_cast<double>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<double>(frame) * kIntensity);
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount,
                                 static_cast<double>(kFrameCount)));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  constexpr double kPitch = 4.0;
  constexpr double kIntensity = 0.25;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0.0);

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
  instrument.SetNoteOn(kPitch + 2.0, kIntensity);
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

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0.0);
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
