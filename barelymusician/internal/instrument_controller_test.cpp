#include "barelymusician/internal/instrument_controller.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::IsNull;
using ::testing::Pointee;
using ::testing::Property;

constexpr int kFrameRate = 4;
constexpr int kChannelCount = 1;
constexpr double kReferenceFrequency = 1.0;

// Tests that the instrument returns a control value as expected.
TEST(InstrumentControllerTest, GetControl) {
  ASSERT_EQ(static_cast<int>(InstrumentControl::kGain), 0);

  InstrumentController instrument(kFrameRate, kReferenceFrequency, 0);
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
TEST(InstrumentControllerTest, PlaySingleNote) {
  constexpr int kFrameCount = 5;
  constexpr double kPitch = 1.0;
  constexpr double kIntensity = 0.5;
  constexpr int64_t kUpdateFrame = 20;

  InstrumentController instrument(kFrameRate, kReferenceFrequency, kUpdateFrame);

  constexpr std::array<double, 4 + kFrameRate> kSampleData = {
      1.0, kPitch, static_cast<double>(kFrameRate), static_cast<double>(kFrameRate), 1.0, 2.0,
      3.0, 4.0,
  };
  instrument.SetData({reinterpret_cast<const std::byte*>(kSampleData.data()),
                      reinterpret_cast<const std::byte*>(kSampleData.data()) +
                          sizeof(double) * kSampleData.size()});

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
                       (frame < kFrameRate) ? kSampleData[frame + 4] * kIntensity : 0.0);
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
  InstrumentController instrument(1, kReferenceFrequency, 0);

  constexpr std::array<double, 1 + kFrameRate * 4> kSampleData = {
      static_cast<double>(kFrameRate),
      // 1
      0.0,
      static_cast<double>(kFrameRate),
      1.0,
      1.0,
      // 2
      1.0,
      static_cast<double>(kFrameRate),
      1.0,
      2.0,
      // 3
      2.0,
      static_cast<double>(kFrameRate),
      1.0,
      3.0,
      // 4
      3.0,
      static_cast<double>(kFrameRate),
      1.0,
      4.0,
  };
  instrument.SetData({reinterpret_cast<const std::byte*>(kSampleData.data()),
                      reinterpret_cast<const std::byte*>(kSampleData.data()) +
                          sizeof(double) * kSampleData.size()});

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
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], static_cast<double>(frame + 1));
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
