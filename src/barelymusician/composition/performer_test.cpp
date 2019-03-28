#include "barelymusician/composition/performer.h"

#include <vector>

#include "barelymusician/base/logging.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to perform.
const int kNumSamples = 16;

// Test note intensity.
const float kNoteIntensity = 1.0f;

// Fake voice that produces constant note index as output for testing.
class FakeInstrument : public Instrument {
 public:
  FakeInstrument() : output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return output_; }
  void Reset() override { output_ = 0.0f; }

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) override {
    LOG(INFO) << "NoteOn(" << index << ", " << intensity << ")";
    output_ = index;
  }
  void NoteOff(float index) override {
    LOG(INFO) << "NoteOff(" << index << ")";
    output_ = 0.0f;
  }
  void SetFloatParam(ParamId id, float value) override {
    LOG(INFO) << "SetFloatParam(" << id << ", " << value << ")";
    output_ = value;
  }

 private:
  float output_;
};

// Tests that performing a single note produces the expected output.
TEST(PerformerTest, PerformSingleNote) {
  const float kNoteIndex = 32.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Play note.
  performer.PlayNote(0, kNoteIndex, kNoteIntensity);

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  // Stop note.
  performer.StopNote(kNumSamples, kNoteIndex);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that performing multiple notes produces the expected output.
TEST(PerformerTest, PerformMultipleNotes) {
  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (const auto& sample : output) {
    EXPECT_FLOAT_EQ(0.0f, sample);
  }

  // Play new note per each sample in the buffer.
  for (int i = 0; i < kNumSamples; ++i) {
    performer.PlayNote(i, static_cast<float>(i), kNoteIntensity);
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i), output[i]);
  }

  // Stop all notes.
  for (int i = 0; i < kNumSamples; ++i) {
    performer.StopNote(kNumSamples, static_cast<float>(i));
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that performing a parameter update produces the expected output.
TEST(PerformerTest, UpdateFloatParam) {
  const ParamId kFloatParamId = 0;
  const float kFloatParamValue = 0.5f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Update float parameter.
  performer.UpdateFloatParam(0, kFloatParamId, kFloatParamValue);

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kFloatParamValue, output[i]);
  }
}

// Tests that resetting the performer clears out the output buffer as expected.
TEST(PerformerTest, Reset) {
  const float kNoteIndex = 1.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  // Play note, then reset.
  performer.PlayNote(0, kNoteIndex, kNoteIntensity);
  performer.Reset();

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

}  // namespace
}  // namespace barelyapi
