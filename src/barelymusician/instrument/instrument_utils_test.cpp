#include "barelymusician/instrument/instrument_utils.h"

#include <vector>

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_buffer.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tolerated error margin.
const float kEpsilon = 1e-1f;

// Process buffer properties.
const int kNumChannels = 1;
const int kNumFrames = 16;

// Test instrument that produces constant output per note for testing.
class TestInstrument : public Instrument {
 public:
  TestInstrument() : sample_(0.0f) {}

  // Implements |Instrument|.
  void AllNotesOff() override {}
  void NoteOff(float) override { sample_ = 0.0f; }
  void NoteOn(float index, float intensity) override {
    sample_ = index * intensity;
  }
  void Process(float* output, int num_channels, int num_frames) override {
    std::fill_n(output, num_channels * num_frames, sample_);
  }

 private:
  float sample_;
};

// Tests that converting arbitrary note indices returns expected frequencies.
TEST(InstrumentUtilsTest, FrequencyFromNoteIndex) {
  const int kNumIndices = 4;
  const float kIndices[kNumIndices] = {21.0f, 60.0f, 69.0f, 90.5f};
  const float kFrequencies[kNumIndices] = {27.5f, 261.6f, 440.0f, 1523.3f};

  for (int i = 0; i < kNumIndices; ++i) {
    EXPECT_NEAR(FrequencyFromNoteIndex(kIndices[i]), kFrequencies[i], kEpsilon);
  }
}

// Tests that processing a single note produces the expected output.
TEST(InstrumentUtilsTest, ProcessSingleNote) {
  const float kNoteIndex = 32.0f;
  const float kNoteIntensity = 0.5f;

  TestInstrument instrument;
  MessageBuffer message_buffer;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  PushNoteOnMessage(kNoteIndex, kNoteIntensity, 0, &message_buffer);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNoteIndex * kNoteIntensity);
    }
  }

  // Stop note.
  PushNoteOffMessage(kNoteIndex, 0, &message_buffer);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that processing multiple notes produces the expected output.
TEST(InstrumentUtilsTest, ProcessMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  TestInstrument instrument;
  MessageBuffer message_buffer;
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    PushNoteOnMessage(static_cast<float>(i), kNoteIntensity, i,
                      &message_buffer);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    PushNoteOffMessage(static_cast<float>(i), 0, &message_buffer);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  Process(&instrument, &message_buffer, buffer.data(), kNumChannels,
          kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

}  // namespace
}  // namespace barelyapi
