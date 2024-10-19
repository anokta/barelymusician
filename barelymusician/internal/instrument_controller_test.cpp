#include "barelymusician/internal/instrument_controller.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/sample_player.h"
#include "barelymusician/internal/sample_data.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kFrameRate = 4;
constexpr int kChannelCount = 1;
constexpr double kReferenceFrequency = 1.0;
constexpr std::array<double, kFrameRate> kSamples = {1.0, 2.0, 3.0, 4.0};

// Tests that the instrument sets a control value as expected.
TEST(InstrumentControllerTest, SetControl) {
  InstrumentController instrument(kFrameRate, kReferenceFrequency, 0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 1.0);

  instrument.SetControl(InstrumentControlType::kGain, 0.25);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 0.25);

  // Verify that the control value is clamped at the minimum value.
  instrument.SetControl(InstrumentControlType::kGain, -2.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 0.0);

  instrument.SetControl(InstrumentControlType::kGain, 0.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 0.0);

  // Verify that the control value is clamped at the maximum value.
  instrument.SetControl(InstrumentControlType::kGain, 5.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 1.0);

  instrument.SetControl(InstrumentControlType::kGain, 1.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(InstrumentControlType::kGain), 1.0);
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentControllerTest, PlaySingleNote) {
  constexpr int kFrameCount = 5;
  constexpr double kPitch = 1.0;
  constexpr double kIntensity = 0.5;
  constexpr int64_t kUpdateFrame = 20;
  constexpr std::array<SampleDataDefinition, 1> kDefinitions = {
      SampleDataDefinition(kPitch, kFrameRate, kSamples)};

  InstrumentController instrument(kFrameRate, kReferenceFrequency, kUpdateFrame);
  instrument.SetControl(InstrumentControlType::kSamplePlaybackMode,
                        static_cast<double>(SamplePlaybackMode::kSustain));
  instrument.SetSampleData(SampleData(kDefinitions));

  std::vector<double> buffer(kChannelCount * kFrameCount);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Set a note on.
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel],
                       (frame < kFrameRate) ? kSamples[frame] * kIntensity : 0.0);
    }
  }

  // Set the note off.
  instrument.SetNoteOff(kPitch);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kUpdateFrame));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that the instrument plays multiple notes as expected.
TEST(InstrumentControllerTest, PlayMultipleNotes) {
  constexpr std::array<SampleDataDefinition, kFrameRate> kDefinitions = {
      SampleDataDefinition(0.0, kFrameRate, {kSamples.data(), kSamples.data() + 1}),
      SampleDataDefinition(1.0, kFrameRate, {kSamples.data() + 1, kSamples.data() + 2}),
      SampleDataDefinition(2.0, kFrameRate, {kSamples.data() + 2, kSamples.data() + 3}),
      SampleDataDefinition(3.0, kFrameRate, {kSamples.data() + 3, kSamples.data() + 4}),
  };

  InstrumentController instrument(1, kReferenceFrequency, 0);
  instrument.SetControl(InstrumentControlType::kSamplePlaybackMode,
                        static_cast<double>(SamplePlaybackMode::kSustain));
  instrument.SetSampleData(SampleData(kDefinitions));

  std::vector<double> buffer(kChannelCount * kFrameRate);

  // Control is set to its default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameRate, 0));
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }

  // Start a new note per each frame in the buffer.
  for (int i = 0; i < kFrameRate; ++i) {
    instrument.SetNoteOn(static_cast<double>(i), 1.0);
    instrument.Update(i + 1);
    instrument.SetNoteOff(static_cast<double>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameRate, 0));
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kSamples[frame]);
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameRate, kFrameRate));
  for (int frame = 0; frame < kFrameRate; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], 0.0);
    }
  }
}

// Tests that the instrument triggers its note callbacks as expected.
TEST(InstrumentControllerTest, SetNoteCallbacks) {
  constexpr double kPitch = 3.3;
  constexpr double kIntensity = 0.25;

  InstrumentController instrument(1, kReferenceFrequency, 0);

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
TEST(InstrumentControllerTest, SetAllNotesOff) {
  constexpr std::array<double, 3> kPitches = {1.0, 2.0, 3.0};
  constexpr double kIntensity = 1.0;

  InstrumentController instrument(kFrameRate, kReferenceFrequency, 0);
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
}  // namespace barely
