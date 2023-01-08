#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/engine/control.h"
#include "barelymusician/engine/status.h"
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
InstrumentDefinition GetTestDefinition() {
  static const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{15.0, 10.0, 20.0},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
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
      [](void** state, double pitch, int index, double value,
         double /*slope_per_frame*/) {
        *reinterpret_cast<double*>(*state) =
            pitch * static_cast<double>(index + 1) * value;
      },
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch) {
        *reinterpret_cast<double*>(*state) = pitch;
      },
      control_definitions, note_control_definitions);
}

// Tests that instrument returns control as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate);

  const auto control_or = instrument.GetControl(0);
  ASSERT_TRUE(control_or.IsOk());
  const auto& control = control_or.GetValue().get();
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_EQ(instrument.SetControl(0, 20.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  EXPECT_EQ(instrument.ResetControl(0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_EQ(instrument.SetControl(0, 50.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  instrument.ResetAllControls();
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  // Control does not exist.
  const auto invalid_control_or = instrument.GetControl(1);
  ASSERT_FALSE(invalid_control_or.IsOk());
  EXPECT_EQ(invalid_control_or.GetErrorStatus(), Status::kInvalidArgument);

  EXPECT_EQ(instrument.SetControl(1, 2.0, 0.0), Status::kInvalidArgument);
  EXPECT_EQ(instrument.ResetControl(1), Status::kInvalidArgument);
}

// Tests that instrument returns note control as expected.
TEST(InstrumentTest, GetNoteControl) {
  const double kPitch = 1.0;

  Instrument instrument(GetTestDefinition(), kFrameRate);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  {
    // Note control is not on.
    const auto not_found_note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_FALSE(not_found_note_control_or.IsOk());
    EXPECT_EQ(not_found_note_control_or.GetErrorStatus(), Status::kNotFound);
  }

  instrument.SetNoteOn(kPitch);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
  ASSERT_TRUE(note_control_or.IsOk());
  const auto& note_control = note_control_or.GetValue().get();
  EXPECT_DOUBLE_EQ(note_control.Get(), 1.0);

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, 0.25, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.Get(), 0.25);

  EXPECT_EQ(instrument.ResetNoteControl(kPitch, 0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.Get(), 1.0);

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, -10.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.Get(), 0.0);

  instrument.ResetAllNoteControls(kPitch);
  EXPECT_DOUBLE_EQ(note_control.Get(), 1.0);

  // Note control does not exist.
  const auto invalid_note_control_or = instrument.GetNoteControl(kPitch, 1);
  ASSERT_FALSE(invalid_note_control_or.IsOk());
  EXPECT_EQ(invalid_note_control_or.GetErrorStatus(), Status::kInvalidArgument);

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 1, 0.25, 0.0),
            Status::kInvalidArgument);
  EXPECT_EQ(instrument.ResetNoteControl(kPitch, 1), Status::kInvalidArgument);

  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  {
    // Note control is not on.
    const auto not_found_note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_FALSE(not_found_note_control_or.IsOk());
    EXPECT_EQ(not_found_note_control_or.GetErrorStatus(), Status::kNotFound);
  }
}

// Tests that instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  const double kPitch = 32.0;
  const double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  instrument.Update(kTimestamp);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Set note on.
  instrument.SetNoteOn(kPitch);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch);
    }
  }

  // Set note off.
  instrument.SetNoteOff(kPitch);
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
    instrument.SetNoteOn(static_cast<double>(i));
    instrument.Update(static_cast<double>(i + 1));
    instrument.SetNoteOff(static_cast<double>(i));
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
  instrument.SetNoteOnEventCallback(
      [&](double pitch) { note_on_pitch = pitch; });
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);

  instrument.SetNoteOn(kPitch);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);

  // This should not trigger the callback since the note is already on.
  note_on_pitch = 0.0;
  instrument.SetNoteOn(kPitch);
  EXPECT_DOUBLE_EQ(note_on_pitch, 0.0);

  // Trigger note on callback again with another note.
  note_on_pitch = 0.0;
  instrument.SetNoteOn(kPitch + 2.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch + 2.0);

  // Trigger note off callback.
  double note_off_pitch = 0.0;
  instrument.SetNoteOffEventCallback(
      [&](double pitch) { note_off_pitch = pitch; });
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

// Tests that instrument stops all notes as expected.
TEST(InstrumentTest, SetAllNotesOff) {
  const std::vector<double> kPitches = {1.0, 2.0, 3.0};

  Instrument instrument(GetTestDefinition(), kFrameRate);
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const double pitch : kPitches) {
    instrument.SetNoteOn(pitch);
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
