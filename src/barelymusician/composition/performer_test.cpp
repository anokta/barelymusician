#include "barelymusician/composition/performer.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/instrument/instrument.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

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

// Tests that performing a single note produces the expected output.
TEST(PerformerTest, PlaySingleNote) {
  const float kNoteIndex = 32.0f;
  const float kNoteIntensity = 0.5f;
  const int kNoteTimestamp = 24;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, kNoteTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  performer.StartNote(kNoteIndex, kNoteIntensity, kNoteTimestamp);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, kNoteTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNoteIndex * kNoteIntensity);
    }
  }

  // Stop note.
  performer.StopNote(kNoteIndex, kNoteTimestamp);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, kNoteTimestamp);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that performing multiple notes produces the expected output.
TEST(PerformerTest, PlayMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, 0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    performer.StartNote(static_cast<float>(i), kNoteIntensity, i);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, 0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    performer.StopNote(static_cast<float>(i), 0);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames, 0);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

}  // namespace
}  // namespace barelyapi
