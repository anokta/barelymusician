#include "barelymusician/engine/instrument_manager.h"

#include <algorithm>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/instrument/instrument.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kNumChannels = 1;
constexpr int kNumFrames = 16;

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
  void SetParam(int, float) override {}

 private:
  float sample_;
};

// Tests that scheduling a single note produces the expected output.
TEST(InstrumentManagerTest, ScheduleSingleNote) {
  const double kBeginTimestamp = 1.0;
  const double kEndTimestamp = 2.0;
  const float kNoteIndex = 32.0f;
  const float kNoteIntensity = 0.5f;

  InstrumentManager instrument_manager;
  const int instrument_id = instrument_manager.Create<TestInstrument>({});

  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, kBeginTimestamp, kEndTimestamp,
                             buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start note.
  instrument_manager.ScheduleNoteOn(instrument_id, kBeginTimestamp, kNoteIndex,
                                    kNoteIntensity);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, kBeginTimestamp, kEndTimestamp,
                             buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel],
                      kNoteIndex * kNoteIntensity);
    }
  }

  // Stop note.
  instrument_manager.ScheduleNoteOff(instrument_id, kBeginTimestamp,
                                     kNoteIndex);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, kBeginTimestamp, kEndTimestamp,
                             buffer.data(), kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

// Tests that scheduling multiple notes produces the expected output.
TEST(InstrumentManagerTest, ScheduleMultipleNotes) {
  const float kNoteIntensity = 1.0f;

  InstrumentManager instrument_manager;
  const int instrument_id = instrument_manager.Create<TestInstrument>({});

  std::vector<float> buffer(kNumChannels * kNumFrames);

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, 0.0, 1.0, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }

  // Start new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    instrument_manager.ScheduleNoteOn(
        instrument_id, static_cast<double>(i) / static_cast<double>(kNumFrames),
        static_cast<float>(i), kNoteIntensity);
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, 0.0, 1.0, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    const float expected = static_cast<float>(frame) * kNoteIntensity;
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], expected);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    instrument_manager.ScheduleNoteOff(instrument_id, 0.0,
                                       static_cast<float>(i));
  }

  std::fill(buffer.begin(), buffer.end(), 0.0f);
  instrument_manager.Process(instrument_id, 0.0, 1.0, buffer.data(),
                             kNumChannels, kNumFrames);
  for (int frame = 0; frame < kNumFrames; ++frame) {
    for (int channel = 0; channel < kNumChannels; ++channel) {
      EXPECT_FLOAT_EQ(buffer[kNumChannels * frame + channel], 0.0f);
    }
  }
}

}  // namespace
}  // namespace barelyapi
