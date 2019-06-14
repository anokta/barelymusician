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

// Test note intensity.
const float kNoteIntensity = 1.0f;

// Test instrument that produces constant note index as output for testing.
class TestInstrument : public Instrument {
 public:
  TestInstrument() : sample_(0.0f) {}

  // Implements |Instrument|.
  void AllNotesOff() override { sample_ = 0.0f; }
  void NoteOff(float) override { sample_ = 0.0f; }
  void NoteOn(float index, float) override { sample_ = index; }
  void Process(float* output, int num_channels, int num_frames) override {
    std::fill_n(output, num_channels * num_frames, sample_);
  }

 private:
  float sample_;
};

// Tests that clearing all the instrument notes clears out the output buffer as
// expected.
TEST(PerformerTest, ClearAllNotes) {
  const float kNoteIndex = 1.0f;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  // Start note.
  performer.StartNote(kNoteIndex, kNoteIntensity, 0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], kNoteIndex);
    }
  }

  // Then, reset.
  performer.ClearAllNotes();

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing a single note produces the expected output.
TEST(PerformerTest, PlaySingleNote) {
  const float kNoteIndex = 32.0f;

  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  performer.StartNote(kNoteIndex, kNoteIntensity, 0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], kNoteIndex);
    }
  }

  // Stop note.
  performer.StopNote(kNoteIndex, 0);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that playing multiple notes produces the expected output.
TEST(PerformerTest, PlayMultipleNotes) {
  Performer performer(std::make_unique<TestInstrument>());
  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
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
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame);
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    performer.StopNote(static_cast<float>(i), 0);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  performer.Process(buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that the registered note callbacks get triggered as expected.
TEST(PerformerTest, RegisterNoteCallbacks) {
  Performer performer(std::make_unique<TestInstrument>());

  // Register note event callbacks.
  float note_index = 0.0f;
  float note_intensity = 0.0f;
  const auto note_on_callback = [&note_index, &note_intensity](
                                    float index, float intensity) {
    note_index = index;
    note_intensity = intensity;
  };
  const auto note_off_callback = [&note_index, &note_intensity](float index) {
    note_index = index;
    note_intensity = 0.0f;
  };
  performer.RegisterNoteOnCallback(note_on_callback);
  performer.RegisterNoteOffCallback(note_off_callback);

  // Play new note per each two samples in the buffer.
  for (int i = 0; i < 2 * kNumFrames; i += 2) {
    const float index = static_cast<float>(i);
    performer.StartNote(index, kNoteIntensity, i);
    performer.StopNote(index, i + 1);
  }

  std::vector<float> buffer(kNumChannels * kNumFrames, 0.0f);
  for (int i = 0; i < 2 * kNumFrames; i += 2) {
    const float expected_index = static_cast<float>(i);
    performer.Process(buffer.data(), kNumChannels, 1);
    EXPECT_FLOAT_EQ(note_index, expected_index);
    EXPECT_FLOAT_EQ(note_intensity, kNoteIntensity);
    performer.Process(buffer.data(), kNumChannels, 1);
    EXPECT_FLOAT_EQ(note_index, expected_index);
    EXPECT_FLOAT_EQ(note_intensity, 0.0f);
  }
}

}  // namespace
}  // namespace barelyapi
