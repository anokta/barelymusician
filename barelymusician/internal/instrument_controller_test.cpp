#include "barelymusician/internal/instrument_controller.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/sample_data.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::Pair;

constexpr int kFrameRate = 4;
constexpr int kChannelCount = 1;
constexpr double kReferenceFrequency = 1.0;
constexpr std::array<double, kFrameRate> kSamples = {1.0, 2.0, 3.0, 4.0};

// Tests that the instrument sets a control value as expected.
TEST(InstrumentControllerTest, SetControl) {
  InstrumentController instrument(kFrameRate, kReferenceFrequency, 0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 1.0);

  instrument.SetControl(ControlType::kGain, 0.25);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 0.25);

  // Verify that the control value is clamped at the minimum value.
  instrument.SetControl(ControlType::kGain, -2.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 0.0);

  instrument.SetControl(ControlType::kGain, 0.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 0.0);

  // Verify that the control value is clamped at the maximum value.
  instrument.SetControl(ControlType::kGain, 5.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 1.0);

  instrument.SetControl(ControlType::kGain, 1.0);
  EXPECT_DOUBLE_EQ(instrument.GetControl(ControlType::kGain), 1.0);
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentControllerTest, PlaySingleNote) {
  constexpr int kFrameCount = 5;
  constexpr double kPitch = 1.0;
  constexpr double kIntensity = 0.5;
  constexpr int64_t kUpdateFrame = 20;
  constexpr std::array<SampleDataSlice, 1> kSlices = {
      SampleDataSlice(kPitch, kFrameRate, kSamples)};

  InstrumentController instrument(kFrameRate, kReferenceFrequency, kUpdateFrame);
  instrument.SetControl(ControlType::kSamplePlaybackMode,
                        static_cast<double>(SamplePlaybackMode::kSustain));
  instrument.SetSampleData(SampleData(kSlices));

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
  constexpr std::array<SampleDataSlice, kFrameRate> kSlices = {
      SampleDataSlice(0.0, kFrameRate, {kSamples.data(), kSamples.data() + 1}),
      SampleDataSlice(1.0, kFrameRate, {kSamples.data() + 1, kSamples.data() + 2}),
      SampleDataSlice(2.0, kFrameRate, {kSamples.data() + 2, kSamples.data() + 3}),
      SampleDataSlice(3.0, kFrameRate, {kSamples.data() + 3, kSamples.data() + 4}),
  };

  InstrumentController instrument(1, kReferenceFrequency, 0);
  instrument.SetControl(ControlType::kSamplePlaybackMode,
                        static_cast<double>(SamplePlaybackMode::kSustain));
  instrument.SetSampleData(SampleData(kSlices));

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
  std::pair<double, double> note_on_state = {0.0, 0.0};
  const auto note_on_event = NoteOnEvent{
      [](void** state, void* user_data) { *state = user_data; },
      [](void**) {},
      [](void** state, double pitch, double intensity) {
        auto& note_on_state = *static_cast<std::pair<double, double>*>(*state);
        note_on_state.first = pitch;
        note_on_state.second = intensity;
      },
      static_cast<void*>(&note_on_state),
  };
  instrument.SetNoteOnEvent(&note_on_event);
  EXPECT_THAT(note_on_state, Pair(0.0, 0.0));

  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_THAT(note_on_state, Pair(kPitch, kIntensity));

  // This should not trigger the callback since the note is already on.
  note_on_state = {0.0, 0.0};
  instrument.SetNoteOn(kPitch, kIntensity);
  EXPECT_THAT(note_on_state, Pair(0.0, 0.0));

  // Trigger the note on callback again with another note.
  note_on_state = {0.0, 0.0};
  instrument.SetNoteOn(kPitch + 2, kIntensity);
  EXPECT_THAT(note_on_state, Pair(kPitch + 2.0, kIntensity));

  // Trigger the note off callback.
  double note_off_pitch = 0.0;
  const auto note_off_event = NoteOffEvent{
      [](void** state, void* user_data) { *state = user_data; },
      [](void**) {},
      [](void** state, double pitch) {
        auto& note_off_pitch = *static_cast<double*>(*state);
        note_off_pitch = pitch;
      },
      static_cast<void*>(&note_off_pitch),
  };
  instrument.SetNoteOffEvent(&note_off_event);
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
