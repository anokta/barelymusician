#include "barelymusician/instrument/instrument.h"

#include "barelymusician/base/buffer.h"
#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
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
  TestInstrument() : output_(0.0f), Instrument(kNumChannels) {}

 protected:
  // Implements |Instrument|.
  void Clear() override { output_ = 0.0f; }
  float Next(int channel) override {
    DCHECK_LT(channel, kNumChannels);
    return output_;
  }
  void NoteOn(float index, float intensity) override {
    DLOG(INFO) << "NoteOn(" << index << ", " << intensity << ")";
    output_ = index;
  }
  void NoteOff(float index) override {
    DLOG(INFO) << "NoteOff(" << index << ")";
    output_ = 0.0f;
  }

 private:
  float output_;
};

// Tests that playing a single note produces the expected output.
TEST(InstrumentTest, PlaySingleNote) {
  const float kNoteIndex = 32.0f;

  TestInstrument instrument;
  Buffer buffer(kNumChannels, kNumFrames);

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }

  // Start note.
  instrument.StartNote(kNoteIndex, kNoteIntensity);

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(kNoteIndex, sample);
    }
  }

  // Stop note.
  instrument.StopNote(kNoteIndex);

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }
}

// Tests that playing multiple notes produces the expected output.
TEST(InstrumentTest, PlayMultipleNotes) {
  TestInstrument instrument;
  Buffer buffer(kNumChannels, kNumFrames);

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }

  // Play new note per each sample in the buffer.
  for (int i = 0; i < kNumFrames; ++i) {
    instrument.StartNote(static_cast<float>(i), kNoteIntensity, i);
  }

  buffer.clear();
  instrument.Process(&buffer);
  for (int i = 0; i < kNumFrames; ++i) {
    for (const auto& sample : buffer[i]) {
      EXPECT_FLOAT_EQ(static_cast<float>(i), sample);
    }
  }

  // Stop all notes.
  for (int i = 0; i < kNumFrames; ++i) {
    instrument.StopNote(static_cast<float>(i));
  }

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }
}

// Tests that resetting the instrument clears out the output buffer as expected.
TEST(InstrumentTest, Reset) {
  const float kNoteIndex = 1.0f;

  TestInstrument instrument;
  Buffer buffer(kNumChannels, kNumFrames);

  // Play note.
  instrument.StartNote(kNoteIndex, kNoteIntensity);

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(kNoteIndex, sample);
    }
  }

  // Then, reset.
  instrument.Reset();

  buffer.clear();
  instrument.Process(&buffer);
  for (const auto& frame : buffer) {
    for (const auto& sample : frame) {
      EXPECT_FLOAT_EQ(0.0f, sample);
    }
  }
}

}  // namespace
}  // namespace barelyapi
