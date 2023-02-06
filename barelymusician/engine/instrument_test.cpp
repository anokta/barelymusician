#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/engine/control.h"
#include "barelymusician/engine/number.h"
#include "barelymusician/engine/status.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::IsNull;
using ::testing::NotNull;

constexpr Integer kFrameRate = 8000;
constexpr Integer kChannelCount = 1;
constexpr Integer kFrameCount = 4;

constexpr Real kTempo = 60.0;

// Returns test instrument definition that produces constant output per note.
InstrumentDefinition GetTestDefinition() {
  static const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{15.0, 10.0, 20.0},
  };
  static const std::vector<ControlDefinition> note_control_definitions = {
      ControlDefinition{1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
      [](void** state, Integer /*frame_rate*/) {
        *state = static_cast<void*>(new Real{0.0});
      },
      [](void** state) { delete static_cast<Real*>(*state); },
      [](void** state, Real* output_samples, Integer output_channel_count,
         Integer output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<Real*>(*state));
      },
      [](void** state, Integer index, Real value, Real /*slope_per_frame*/) {
        *reinterpret_cast<Real*>(*state) = static_cast<Real>(index + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, Integer /*size*/) {},
      [](void** state, Real pitch, Integer index, Real value,
         Real /*slope_per_frame*/) {
        *reinterpret_cast<Real*>(*state) =
            pitch * static_cast<Real>(index + 1) * value;
      },
      [](void** state, Real /*pitch*/) {
        *reinterpret_cast<Real*>(*state) = 0.0;
      },
      [](void** state, Real pitch) {
        *reinterpret_cast<Real*>(*state) = pitch;
      },
      control_definitions, note_control_definitions);
}

// Tests that instrument returns control as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);

  const auto control_or = instrument.GetControl(0);
  ASSERT_TRUE(control_or.IsOk());
  const auto& control = control_or->get();
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  EXPECT_EQ(instrument.SetControl(0, 20.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  EXPECT_EQ(instrument.ResetControl(0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  EXPECT_EQ(instrument.SetControl(0, 50.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  instrument.ResetAllControls();
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  // Control does not exist.
  const auto invalid_control_or = instrument.GetControl(1);
  ASSERT_FALSE(invalid_control_or.IsOk());
  EXPECT_EQ(invalid_control_or.GetErrorStatus(), Status::kInvalidArgument);

  EXPECT_EQ(instrument.SetControl(1, 2.0, 0.0), Status::kInvalidArgument);
  EXPECT_EQ(instrument.ResetControl(1), Status::kInvalidArgument);
}

// Tests that instrument returns note control as expected.
TEST(InstrumentTest, GetNoteControl) {
  const Real kPitch = 1.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);
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
  const auto& note_control = note_control_or->get();
  EXPECT_DOUBLE_EQ(note_control.GetValue(), 1.0);

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, 0.25, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.GetValue(), 0.25);

  EXPECT_EQ(instrument.ResetNoteControl(kPitch, 0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.GetValue(), 1.0);

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, -10.0, 0.0), Status::kOk);
  EXPECT_DOUBLE_EQ(note_control.GetValue(), 0.0);

  instrument.ResetAllNoteControls(kPitch);
  EXPECT_DOUBLE_EQ(note_control.GetValue(), 1.0);

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
  const Real kPitch = 32.0;
  const Integer kTimestamp = 20'000'000;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, kTimestamp);
  std::vector<Real> buffer(kChannelCount * kFrameCount);

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Set note on.
  instrument.SetNoteOn(kPitch);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch);
    }
  }

  // Set note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  Instrument instrument(GetTestDefinition(), 1, kTempo, 0);
  std::vector<Real> buffer(kChannelCount * kFrameCount);

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Start new note per each frame in the buffer.
  for (Integer i = 0; i < kFrameCount; ++i) {
    instrument.SetNoteOn(static_cast<Real>(i));
    instrument.Update(1'000'000 * (i + 1));
    instrument.SetNoteOff(static_cast<Real>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<Real>(frame));
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount,
                     1'000'000 * kFrameCount);
  for (Integer frame = 0; frame < kFrameCount; ++frame) {
    for (Integer channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that instrument triggers note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  const Real kPitch = 4.0;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0);

  // Trigger note on callback.
  Real note_on_pitch = 0.0;
  instrument.SetNoteOnEventCallback([&](Real pitch) { note_on_pitch = pitch; });
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
  Real note_off_pitch = 0.0;
  instrument.SetNoteOffEventCallback(
      [&](Real pitch) { note_off_pitch = pitch; });
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
  const std::vector<Real> kPitches = {1.0, 2.0, 3.0};

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0);
  for (const Real pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }

  // Start multiple notes.
  for (const Real pitch : kPitches) {
    instrument.SetNoteOn(pitch);
    EXPECT_TRUE(instrument.IsNoteOn(pitch));
  }

  // Stop all notes.
  instrument.SetAllNotesOff();
  for (const Real pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely::internal
