#include "barelymusician/engine/instrument.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/engine/parameter.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::IsNull;
using ::testing::NotNull;

constexpr int kFrameRate = 8000;
constexpr int kNumChannels = 1;
constexpr int kNumFrames = 4;

// Returns test instrument definition that produces constant output per note.
Instrument::Definition GetTestDefinition() {
  static const std::vector<Parameter::Definition> parameter_definitions = {
      Parameter::Definition{15.0, 10.0, 20.0},
  };
  return Instrument::Definition(
      [](void** state, int /*frame_rate*/) {
        *state = static_cast<void*>(new double{0.0});
      },
      [](void** state) { delete static_cast<double*>(*state); },
      [](void** state, double* output, int num_output_channels,
         int num_output_frames) {
        std::fill_n(output, num_output_channels * num_output_frames,
                    *reinterpret_cast<double*>(*state));
      },
      [](void** /*state*/, const void* /*data*/, int /*size*/) {},
      [](void** state, double /*pitch*/) {
        *reinterpret_cast<double*>(*state) = 0.0;
      },
      [](void** state, double pitch, double intensity) {
        *reinterpret_cast<double*>(*state) = pitch * intensity;
      },
      [](void** state, int index, double value, double /*slope*/) {
        *reinterpret_cast<double*>(*state) =
            static_cast<double>(index + 1) * value;
      },
      parameter_definitions);
}

// Tests that instrument returns parameter as expected.
TEST(InstrumentTest, GetParameter) {
  Instrument instrument(GetTestDefinition(), kFrameRate);

  const auto* parameter = instrument.GetParameter(0);
  EXPECT_THAT(parameter, NotNull());

  const auto& definition = parameter->GetDefinition();
  EXPECT_DOUBLE_EQ(definition.default_value, 15.0);
  EXPECT_DOUBLE_EQ(definition.min_value, 10.0);
  EXPECT_DOUBLE_EQ(definition.max_value, 20.0);

  EXPECT_DOUBLE_EQ(parameter->GetValue(), 15.0);

  EXPECT_TRUE(instrument.SetParameter(0, 20.0, 0.0, 0.0));
  EXPECT_DOUBLE_EQ(parameter->GetValue(), 20.0);

  EXPECT_TRUE(instrument.ResetParameter(0, 0.0));
  EXPECT_DOUBLE_EQ(parameter->GetValue(), 15.0);

  EXPECT_TRUE(instrument.SetParameter(0, 50.0, 0.0, 0.0));
  EXPECT_DOUBLE_EQ(parameter->GetValue(), 20.0);

  instrument.ResetAllParameters(0.0);
  EXPECT_DOUBLE_EQ(parameter->GetValue(), 15.0);

  // Parameter does not exist.
  EXPECT_THAT(instrument.GetParameter(1), IsNull());
}

// Tests that instrument plays a single note as expected.
TEST(InstrumentTest, PlaySingleNote) {
  const double kPitch = 32.0;
  const double kIntensity = 0.5;
  const double kTimestamp = 20.0;

  Instrument instrument(GetTestDefinition(), kFrameRate);
  std::vector<double> buffer(kNumChannels * kNumFrames);

  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  // Parameter is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 15.0);
    }
  }

  // Start note.
  instrument.StartNote(kPitch, kIntensity, kTimestamp);
  EXPECT_TRUE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel],
                       kPitch * kIntensity);
    }
  }

  // Stop note.
  instrument.StopNote(kPitch, kTimestamp);
  EXPECT_FALSE(instrument.IsNoteOn(kPitch));

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames, kTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 0.0);
    }
  }
}

// Tests that instrument plays multiple notes as expected.
TEST(InstrumentTest, PlayMultipleNotes) {
  const double kIntensity = 1.0;

  Instrument instrument(GetTestDefinition(), 1);
  std::vector<double> buffer(kNumChannels * kNumFrames);

  // Parameter is set to default value.
  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 15.0);
    }
  }

  // Start new note per each frame in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    instrument.StartNote(static_cast<double>(i), kIntensity,
                         static_cast<double>(i));
    instrument.StopNote(static_cast<double>(i), static_cast<double>(i + 1));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames, 0.0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const double expected = static_cast<double>(frame) * kIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  std::fill(buffer.begin(), buffer.end(), 0.0);
  instrument.Process(buffer.data(), kNumChannels, kNumFrames,
                     static_cast<double>(kNumFrames));
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_DOUBLE_EQ(buffer[kNumChannels * frame + channel], 0.0);
    }
  }
}

// Tests that instrument triggers note callbacks as expected.
TEST(InstrumentTest, SetNoteCallbacks) {
  const double kPitch = 4.0;
  const double kIntensity = 0.25;

  Instrument instrument(GetTestDefinition(), 1);

  // Trigger note on callback.
  double note_on_pitch = 0.0;
  double note_on_intensity = 0.0;
  double note_on_timestamp = 0.0;
  instrument.SetNoteOnCallback(
      [&](double pitch, double intensity, double timestamp) {
        note_on_pitch = pitch;
        note_on_intensity = intensity;
        note_on_timestamp = timestamp;
      });
  EXPECT_NE(note_on_pitch, kPitch);
  EXPECT_NE(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 0.0);

  instrument.StartNote(kPitch, kIntensity, 10.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);

  // This should not trigger the callback since the note is already on.
  instrument.StartNote(kPitch, kIntensity, 15.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 10.0);

  // Trigger note on callback again with another note.
  instrument.StartNote(kPitch + 2.0, kIntensity, 15.0);
  EXPECT_DOUBLE_EQ(note_on_pitch, kPitch + 2.0);
  EXPECT_DOUBLE_EQ(note_on_intensity, kIntensity);
  EXPECT_DOUBLE_EQ(note_on_timestamp, 15.0);

  // Trigger note off callback.
  double note_off_pitch = 0.0;
  double note_off_timestamp = 0.0;
  instrument.SetNoteOffCallback([&](double pitch, double timestamp) {
    note_off_pitch = pitch;
    note_off_timestamp = timestamp;
  });
  EXPECT_NE(note_off_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 0.0);

  instrument.StopNote(kPitch, 20.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);

  // This should not trigger the callback since the note is already off.
  instrument.StopNote(kPitch, 25.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 20.0);

  // Finally, trigger the note off callback with the remaining note.
  instrument.StopAllNotes(25.0);
  EXPECT_DOUBLE_EQ(note_off_pitch, kPitch + 2.0);
  EXPECT_DOUBLE_EQ(note_off_timestamp, 25.0);
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
    instrument.StartNote(pitch, 1.0, 0.0);
    EXPECT_TRUE(instrument.IsNoteOn(1.0));
  }

  // Stop all notes.
  instrument.StopAllNotes(0.0);
  for (const double pitch : kPitches) {
    EXPECT_FALSE(instrument.IsNoteOn(pitch));
  }
}

}  // namespace
}  // namespace barely::internal
