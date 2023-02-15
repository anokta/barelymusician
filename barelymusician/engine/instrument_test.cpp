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

constexpr double kTempo = 60.0;

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
      [](void** /*state*/, double /*pitch*/, int /*index*/, double /*value*/,
         double /*slope_per_frame*/) {},
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      control_definitions, note_control_definitions);
}

// Tests that instrument returns control as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0.0);

  {
    const auto control_or = instrument.GetControl(0);
    ASSERT_TRUE(control_or.IsOk());
    EXPECT_DOUBLE_EQ(*control_or, 15.0);
  }

  EXPECT_EQ(instrument.SetControl(0, 20.0, 0.0), Status::Ok());
  {
    const auto control_or = instrument.GetControl(0);
    ASSERT_TRUE(control_or.IsOk());
    EXPECT_DOUBLE_EQ(*control_or, 20.0);
  }

  EXPECT_EQ(instrument.ResetControl(0), Status::Ok());
  {
    const auto control_or = instrument.GetControl(0);
    ASSERT_TRUE(control_or.IsOk());
    EXPECT_DOUBLE_EQ(*control_or, 15.0);
  }

  EXPECT_EQ(instrument.SetControl(0, 50.0, 0.0), Status::Ok());
  {
    const auto control_or = instrument.GetControl(0);
    ASSERT_TRUE(control_or.IsOk());
    EXPECT_DOUBLE_EQ(*control_or, 20.0);
  }

  instrument.ResetAllControls();
  {
    const auto control_or = instrument.GetControl(0);
    ASSERT_TRUE(control_or.IsOk());
    EXPECT_DOUBLE_EQ(*control_or, 15.0);
  }

  // Control does not exist.
  const auto invalid_control_or = instrument.GetControl(1);
  ASSERT_FALSE(invalid_control_or.IsOk());
  EXPECT_EQ(invalid_control_or.GetErrorStatus(), Status::InvalidArgument());

  EXPECT_EQ(instrument.SetControl(1, 2.0, 0.0), Status::InvalidArgument());
  EXPECT_EQ(instrument.ResetControl(1), Status::InvalidArgument());
}

// Tests that instrument returns note control as expected.
TEST(InstrumentTest, GetNoteControl) {
  const double kPitch = 1.0;
  const double kIntensity = 1.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, 0.0);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  {
    // Note control is not on.
    const auto not_found_note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_FALSE(not_found_note_control_or.IsOk());
    EXPECT_EQ(not_found_note_control_or.GetErrorStatus(), Status::kNotFound);
  }

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  {
    const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_TRUE(note_control_or.IsOk());
    EXPECT_DOUBLE_EQ(*note_control_or, 1.0);
  }

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, 0.25, 0.0), Status::Ok());
  {
    const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_TRUE(note_control_or.IsOk());
    EXPECT_DOUBLE_EQ(*note_control_or, 0.25);
  }

  EXPECT_EQ(instrument.ResetNoteControl(kPitch, 0), Status::Ok());
  {
    const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_TRUE(note_control_or.IsOk());
    EXPECT_DOUBLE_EQ(*note_control_or, 1.0);
  }

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 0, -10.0, 0.0), Status::Ok());
  {
    const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_TRUE(note_control_or.IsOk());
    EXPECT_DOUBLE_EQ(*note_control_or, 0.0);
  }

  instrument.ResetAllNoteControls(kPitch);
  {
    const auto note_control_or = instrument.GetNoteControl(kPitch, 0);
    ASSERT_TRUE(note_control_or.IsOk());
    EXPECT_DOUBLE_EQ(*note_control_or, 1.0);
  }

  // Note control does not exist.
  const auto invalid_note_control_or = instrument.GetNoteControl(kPitch, 1);
  ASSERT_FALSE(invalid_note_control_or.IsOk());
  EXPECT_EQ(invalid_note_control_or.GetErrorStatus(),
            Status::InvalidArgument());

  EXPECT_EQ(instrument.SetNoteControl(kPitch, 1, 0.25, 0.0),
            Status::InvalidArgument());
  EXPECT_EQ(instrument.ResetNoteControl(kPitch, 1), Status::InvalidArgument());

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
  const double kIntensity = 0.5;
  const double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTempo, kTimestamp);
  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 15.0);
    }
  }

  // Set note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       kPitch * kIntensity);
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
  const double kIntensity = 1.0;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0.0);
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
    instrument.SetNoteOn(static_cast<double>(i), kIntensity);
    instrument.Update(static_cast<double>(i + 1));
    instrument.SetNoteOff(static_cast<double>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kChannelCount, kFrameCount, 0.0);
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       static_cast<double>(frame) * kIntensity);
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
  const double kIntensity = 0.25;

  Instrument instrument(GetTestDefinition(), 1, kTempo, 0.0);

  // Trigger note on callback.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  instrument.SetNoteOnEventCallback([&](double pitch, double intensity) {
    note_on_pitch = pitch;
    note_on_intensity = intensity;
  });
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

  // Trigger note on callback again with another note.
  note_on_pitch = 0.0;
  note_on_intensity = 0.0;
  instrument.SetNoteOn(kPitch + 2.0, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch + 2.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);

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
  const double kIntensity = 1.0;

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
