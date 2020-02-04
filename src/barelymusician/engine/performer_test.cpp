#include "barelymusician/engine/performer.h"

#include <algorithm>
#include <memory>
#include <vector>

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
TEST(PerformerTest, ScheduleSingleNote) {
  const float kNoteIndex = 32.0f;
  const float kNoteIntensity = 0.5f;
  const double kStartPosition = 1.5f;
  const double kEndPosition = 2.0f;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(kStartPosition, kEndPosition, buffer.data(), kNumChannels,
                    kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  performer.ScheduleNoteOn(kNoteIndex, kNoteIntensity, kStartPosition);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(kStartPosition, kEndPosition, buffer.data(), kNumChannels,
                    kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNoteIndex * kNoteIntensity);
    }
  }

  // Stop note.
  performer.ScheduleNoteOff(kNoteIndex, kStartPosition);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(kStartPosition, kEndPosition, buffer.data(), kNumChannels,
                    kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that performing multiple notes produces the expected output.
TEST(PerformerTest, ScheduleMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(0.0, 1.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    performer.ScheduleNoteOn(
        static_cast<float>(i), kNoteIntensity,
        static_cast<double>(i) / static_cast<double>(kNumFrames));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(0.0, 1.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  performer.AllScheduledNotesOff();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(0.0, 1.0, buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that performing notes triggers the corresponding callbacks.
TEST(PerformerTest, SetNoteCallbacks) {
  const float kNoteIndex = 40.0f;
  const float kNoteIntensity = 0.75f;

  Performer performer(std::make_unique<TestInstrument>());

  // Perform note on.
  float note_on_index = 0.0f;
  float note_on_intensity = 0.0f;
  performer.SetNoteOnCallback([&](float index, float intensity) {
    note_on_index = index;
    note_on_intensity = intensity;
  });
  EXPECT_NE(note_on_index, kNoteIndex);
  EXPECT_NE(note_on_intensity, kNoteIntensity);

  performer.NoteOn(kNoteIndex, kNoteIntensity);
  EXPECT_FLOAT_EQ(note_on_index, kNoteIndex);
  EXPECT_FLOAT_EQ(note_on_intensity, kNoteIntensity);

  // Perform note off.
  float note_off_index = 0.0f;
  performer.SetNoteOffCallback([&](float index) { note_off_index = index; });
  EXPECT_NE(note_off_index, kNoteIndex);

  performer.NoteOff(kNoteIndex);
  EXPECT_FLOAT_EQ(note_off_index, kNoteIndex);
}

}  // namespace
}  // namespace barelyapi