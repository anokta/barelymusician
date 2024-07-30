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
using ::testing::NotNull;
using ::testing::Pointee;
using ::testing::Property;

constexpr int kFrameRate = 8000;
constexpr int kChannelCount = 1;
constexpr int kFrameCount = 4;
constexpr double kPitch = 440.0;
constexpr double kIntensity = 0.5;

// Returns a test instrument definition that produces constant output per note.
InstrumentDefinition GetTestDefinition() {
  static const std::array<ControlDefinition, 1> control_definitions = {
      ControlDefinition{0, 15.0, 10.0, 20.0},
  };
  static const std::array<ControlDefinition, 1> note_control_definitions = {
      ControlDefinition{0, 1.0, 0.0, 1.0},
  };
  return InstrumentDefinition(
      [](void** state, int /*frame_rate*/) { *state = static_cast<void*>(new double{0.0}); },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output_samples, int32_t output_channel_count,
         int32_t output_frame_count) {
        std::fill_n(output_samples, output_channel_count * output_frame_count,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** state, int32_t control_id, double value) {
        *reinterpret_cast<double*>(*state) = static_cast<double>(control_id + 1) * value;
      },
      [](void** /*state*/, const void* /*data*/, int32_t /*size*/) {},
      [](void** /*state*/, int /*note_id*/, int32_t /*control_id*/, double /*value*/) {},
      [](void** state, int /*note_id*/) { *reinterpret_cast<double*>(*state) = 0.0; },
      [](void** state, int /*note_id*/, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      control_definitions, note_control_definitions);
}

// Tests that the instrument returns a control value as expected.
TEST(InstrumentTest, GetControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, 0.0);
  EXPECT_THAT(instrument.GetControl(0), Pointee(Property(&Control::GetValue, 15.0)));

  // Control does not exist.
  EXPECT_THAT(instrument.GetControl(1), IsNull());
}

// Tests that the instrument returns a note control value as expected.
TEST(InstrumentTest, GetNoteControl) {
  Instrument instrument(GetTestDefinition(), kFrameRate, 0.0);

  Note* note = instrument.CreateNote(kPitch, kIntensity);
  ASSERT_THAT(note, NotNull());
  EXPECT_THAT(note->GetControl(0), Pointee(Property(&Control::GetValue, 1.0)));

  instrument.DestroyNote(note);
}

// Tests that the instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  constexpr double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate, kTimestamp);
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
  Note* note = instrument.CreateNote(kPitch, kIntensity);
  ASSERT_THAT(note, NotNull());

  std::fill(buffer.begin(), buffer.end(), 0.0);
  EXPECT_TRUE(instrument.Process(buffer.data(), kChannelCount, kFrameCount, kTimestamp));
  for (int frame = 0; frame < kFrameCount; ++frame) {
    for (int channel = 0; channel < kChannelCount; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kChannelCount * frame + channel], kPitch * kIntensity);
    }
  }

  // Set the note off.
  instrument.DestroyNote(note);

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
  Instrument instrument(GetTestDefinition(), 1, 0.0);
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
    Note* note = instrument.CreateNote(static_cast<double>(i), kIntensity);
    instrument.Update(static_cast<double>(i + 1));
    instrument.DestroyNote(note);
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

}  // namespace
}  // namespace barely::internal
