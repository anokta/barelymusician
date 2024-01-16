#include "barelymusician/internal/instrument.h"

#include <stdint.h>  // NOLINT(modernize-deprecated-headers)

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

constexpr int kTempo = 60;

// Returns a test instrument definition that produces constant output per note.
InstrumentDefinition GetTestDefinition() {
  static const std::array<ControlDefinition, 1> control_definitions = {
      ControlDefinition{15, 10, 20},
  };
  static const std::array<ControlDefinition, 1> note_control_definitions = {
      ControlDefinition{1, 0, 1},
  };
  return InstrumentDefinition(
      [](void** state, int /*frame_rate*/) { *state = static_cast<void*>(new double{0.0}); },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int32_t output_channel_count,
         int32_t output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int32_t index, BarelyRational value, BarelyRational /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) = static_cast<double>((index + 1) * value);
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {},
      [](void** /*state*/, BarelyRational /*pitch*/, int32_t /*index*/, BarelyRational /*value*/,
         BarelyRational /*slope_per_frame*/) {},
      [](void** state, BarelyRational /*pitch*/) { *reinterpret_cast<double*>(*state) = 0.0; },
      [](void** state, BarelyRational pitch, BarelyRational intensity) {
        *reinterpret_cast<double*>(*state) = static_cast<double>(pitch * intensity);
      },
      control_definitions, note_control_definitions);
}

// Tests that the instrument returns a control value as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, Rational(15))));

  EXPECT_TRUE(instrument.SetControl(0, 20, 0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, Rational(20))));

  EXPECT_TRUE(instrument.ResetControl(0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, Rational(15))));

  EXPECT_TRUE(instrument.SetControl(0, 50, 0));
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, Rational(20))));

  instrument.ResetAllControls();
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, Rational(15))));

  // Control does not exist.
  EXPECT_THAT(instrument.GetControl(1), IsNull());
  EXPECT_FALSE(instrument.SetControl(1, 2, 0));
  EXPECT_FALSE(instrument.ResetControl(1));
}

// Tests that the instrument returns a note control value as expected.
TEST(InstrumentTest, GetNoteControl) {
  constexpr Rational kPitch = 10;
  constexpr Rational kIntensity = 1;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));
  EXPECT_FALSE(instrument.GetNoteControl(kPitch, 0));

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0),
              Pointee(Property(&Control::GetValue, Rational(1))));

  EXPECT_TRUE(instrument.SetNoteControl(kPitch, 0, Rational(1, 4), 0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0),
              Pointee(Property(&Control::GetValue, Rational(1, 4))));

  EXPECT_TRUE(instrument.ResetNoteControl(kPitch, 0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0),
              Pointee(Property(&Control::GetValue, Rational(1))));

  EXPECT_TRUE(instrument.SetNoteControl(kPitch, 0, -10, 0));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0),
              Pointee(Property(&Control::GetValue, Rational(0))));

  instrument.ResetAllNoteControls(kPitch);
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0),
              Pointee(Property(&Control::GetValue, Rational(1))));

  // Note control does not exist.
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 1), IsNull());
  EXPECT_FALSE(instrument.SetNoteControl(kPitch, 1, Rational(1, 4), 0));
  EXPECT_FALSE(instrument.ResetNoteControl(kPitch, 1));

  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));
  EXPECT_THAT(instrument.GetNoteControl(kPitch, 0), IsNull());
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  constexpr Rational kPitch = 32;
  constexpr Rational kIntensity = Rational(1, 2);
  constexpr std::int64_t kTimestamp = 20;

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
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<double>(kPitch * kIntensity));
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
  constexpr Rational kIntensity = 1;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Start a new note per each frame in the buffer.
  for (int i = 0; i < kFrameCount; ++i) {
    instrument.SetNoteOn(i, kIntensity);
    instrument.Update(i + 1, 0);
    instrument.SetNoteOff(i);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<double>(frame * kIntensity));
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kFrameCount));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  constexpr Rational kPitch = 4;
  constexpr Rational kIntensity = Rational(1, 4);

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0);

  // Trigger the note on callback.
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

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_EQ(note_on_pitch, kPitch);
  EXPECT_EQ(note_on_intensity, kIntensity);

  // This should not trigger the callback since the note is already on.
  note_on_pitch = 0;
  note_on_intensity = 0;
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_EQ(note_on_pitch, 0);
  EXPECT_EQ(note_on_intensity, 0);

  // Trigger the note on callback again with another note.
  note_on_pitch = 0;
  note_on_intensity = 0;
  instrument.SetNoteOn(kPitch + 2, kIntensity);
  EXPECT_EQ(note_on_pitch, kPitch + 2);
  EXPECT_EQ(note_on_intensity, kIntensity);

  // Trigger the note off callback.
  Rational note_off_pitch = 0;
  NoteOffEventDefinition::Callback note_off_callback = [&](Rational pitch) {
    note_off_pitch = pitch;
  };
  instrument.SetNoteOffEvent(NoteOffEventDefinition::WithCallback(),
                             static_cast<void*>(&note_off_callback));
  EXPECT_EQ(note_off_pitch, 0);

  instrument.SetNoteOff(kPitch);
  EXPECT_EQ(note_off_pitch, kPitch);

  // This should not trigger the callback since the note is already off.
  note_off_pitch = 0;
  instrument.SetNoteOff(kPitch);
  EXPECT_EQ(note_off_pitch, 0);

  // Finally, trigger the note off callback with the remaining note.
  instrument.SetAllNotesOff();
  EXPECT_EQ(note_off_pitch, kPitch + 2);
}

// Tests that the instrument stops all notes as expected.
TEST(InstrumentTest, SetAllNotesOff) {
  constexpr std::array<Rational, 3> kPitches = {1, 2, 3};
  constexpr Rational kIntensity = 1;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);
  for (const Rational pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const Rational pitch : kPitches) {
    instrument.SetNoteOn(pitch, kIntensity);
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.SetAllNotesOff();
  for (const Rational pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely::internal
