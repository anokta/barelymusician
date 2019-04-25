#include "barelymusician/ensemble/performer.h"

#include <vector>

#include "barelymusician/base/logging.h"
#include "barelymusician/instrument/instrument.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to process.
const int kNumSamples = 16;

// Test note intensity.
const float kNoteIntensity = 1.0f;

// Fake instrument that produces constant note index as output for testing.
class FakeInstrument : public Instrument {
 public:
  FakeInstrument() : output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return output_; }
  void Reset() override { output_ = 0.0f; }

  // Implements |Instrument|.
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

// Tests that playing a note with duration produces the expected output.
TEST(PerformerTest, PlayNoteWithDuration) {
  const float kNoteIndex = 64.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Play note for |kNumSamples| duration.
  performer.PlayNote(0, kNumSamples, kNoteIndex, kNoteIntensity);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that playing a single note produces the expected output.
TEST(PerformerTest, PlaySingleNote) {
  const float kNoteIndex = 32.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Play note.
  performer.NoteOn(0, kNoteIndex, kNoteIntensity);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  // Stop note.
  performer.NoteOff(0, kNoteIndex);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that playing multiple notes produces the expected output.
TEST(PerformerTest, PlayMultipleNotes) {
  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (const auto& sample : output) {
    EXPECT_FLOAT_EQ(0.0f, sample);
  }

  // Play new note per each sample in the buffer.
  for (int i = 0; i < kNumSamples; ++i) {
    performer.NoteOn(i, static_cast<float>(i), kNoteIntensity);
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i), output[i]);
  }

  // Stop all notes.
  for (int i = 0; i < kNumSamples; ++i) {
    performer.NoteOff(0, static_cast<float>(i));
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that resetting the processor clears out the output buffer as expected.
TEST(PerformerTest, Reset) {
  const float kNoteIndex = 1.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  // Play note, then reset.
  performer.NoteOn(0, kNoteIndex, kNoteIntensity);
  performer.Reset();

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

}  // namespace
}  // namespace barelyapi
