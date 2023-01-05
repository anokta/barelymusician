#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/engine/control.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::IsNull;
using ::testing::NotNull;

constexpr int kFrameRate = 8000;
constexpr int kChannelCount = 1;
constexpr int kFrameCount = 4;

// Returns test instrument definition that produces constant output per note.
Instrument::Definition GetTestDefinition() {
  static const std::vector<Control::Definition> control_definitions = {
      Control::Definition{15.0, 10.0, 20.0},
  };
  return Instrument::Definition(
      [](void** state, int /*frame_rate*/) {
        *state = static_cast<void*>(new double{0.0});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int output_channel_count,
         int output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int index, double value, double /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) =
            static_cast<double>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int /*size*/) {},
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch) {
        *reinterpret_cast<double*>(*state) = pitch;
      },
      control_definitions);
}

// Tests that instrument returns control as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate);

  const auto* control = instrument.GetControl(0);
  EXPECT_THAT(control, NotNull());

  const auto& definition = control->GetDefinition();
  EXPECT_DOUBLE_EQ(definition.default_value, 15.0);
  EXPECT_DOUBLE_EQ(definition.min_value, 10.0);
  EXPECT_DOUBLE_EQ(definition.max_value, 20.0);

  EXPECT_DOUBLE_EQ(control->GetValue(), 15.0);

  EXPECT_TRUE(instrument.SetControl(0, 20.0, 0.0, 0.0));
  EXPECT_DOUBLE_EQ(control->GetValue(), 20.0);

  EXPECT_TRUE(instrument.ResetControl(0, 0.0));
  EXPECT_DOUBLE_EQ(control->GetValue(), 15.0);

  EXPECT_TRUE(instrument.SetControl(0, 50.0, 0.0, 0.0));
  EXPECT_DOUBLE_EQ(control->GetValue(), 20.0);

  instrument.ResetAllControls(0.0);
  EXPECT_DOUBLE_EQ(control->GetValue(), 15.0);

  // Control does not exist.
  EXPECT_THAT(instrument.GetControl(1), IsNull());
}

// Tests that instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  const double kPitch = 32.0;
  const double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Start note.
  instrument.StartNote(kPitch, kTimestamp);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch);
    }
  }

  // Stop note.
  instrument.StopNote(kPitch, kTimestamp);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  Instrument instrument(GetTestDefinition(), 1);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Start new note per each frame in the buffer.
  for (int i = 0; i < kFrameCount; ++i) {
    instrument.StartNote(static_cast<double>(i), static_cast<double>(i));
    instrument.StopNote(static_cast<double>(i), static_cast<double>(i + 1));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<double>(frame));
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount,
                     static_cast<double>(kFrameCount));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that instrument triggers note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  const double kPitch = 4.0;

  Instrument instrument(GetTestDefinition(), 1);

  // Trigger note on callback.
  double note_on_pitch = 0.0;
  instrument.SetNoteOnCallback([&](double pitch) { note_on_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);

  instrument.StartNote(kPitch, 10.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);

  // This should not trigger the callback since the note is already on.
  note_on_pitch = 0.0;
  instrument.StartNote(kPitch, 15.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);

  // Trigger note on callback again with another note.
  note_on_pitch = 0.0;
  instrument.StartNote(kPitch + 2.0, 15.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch + 2.0);

  // Trigger note off callback.
  double note_off_pitch = 0.0;
  instrument.SetNoteOffCallback([&](double pitch) { note_off_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  instrument.StopNote(kPitch, 20.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);

  // This should not trigger the callback since the note is already off.
  note_off_pitch = 0.0;
  instrument.StopNote(kPitch, 25.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, 0.0);

  // Finally, trigger the note off callback with the remaining note.
  instrument.StopAllNotes(25.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch + 2.0);
}

// Tests that instrument stops all notes as expected.
TEST(InstrumentTest, StopAllNotes) {
  const std::vector<double> kPitches = {1.0, 2.0, 3.0};

  Instrument instrument(GetTestDefinition(), kFrameRate);
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const double pitch : kPitches) {
    instrument.StartNote(pitch, 0.0);
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.StopAllNotes(0.0);
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely::internal
