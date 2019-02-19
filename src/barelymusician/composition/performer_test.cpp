#include "barelymusician/composition/performer.h"

#include <vector>

#include "barelymusician/base/logging.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to perform.
constexpr int kNumSamples = 16;

// Test note intensity.
constexpr float kNoteIntensity = 1.0f;

// Fake voice that produces constant note index as output for testing.
class FakeInstrument : public Instrument {
 public:
  FakeInstrument() : output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return output_; }
  void Reset() override { output_ = 0.0f; }

  // Implements |Instrument|.
  void NoteOn(float index, float intensity) {
    LOG(INFO) << "NoteOn(" << index << ", " << intensity << ")";
    output_ = index;
  }
  void NoteOff(float index) {
    LOG(INFO) << "NoteOff(" << index << ")";
    output_ = 0.0f;
  }
  float GetFloatParam(int id) const {
    LOG(INFO) << "GetFloatParam(" << id << ")";
    return 0.0f;
  }
  void SetFloatParam(int id, float value) {
    LOG(INFO) << "SetFloatParam(" << id << ", " << value << ")";
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

  // Perform note on.
  performer.Perform(true, kNoteIndex, kNoteIntensity, 0);

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  // Perform note off.
  performer.Perform(false, kNoteIndex, kNoteIntensity, kNumSamples);

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

  // Perform a new note per each sample in the buffer.
  for (int i = 0; i < kNumSamples; ++i) {
    performer.Perform(true, static_cast<float>(i), kNoteIntensity, i);
  }

  output.assign(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i), output[i]);
  }

  // Perform note off.
  performer.Perform(false, 0.0f, kNoteIntensity, kNumSamples);

  output.assign(kNumSamples, 0.0f);
  performer.Process(kNumSamples, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that resetting the performer clears out the output buffer as expected.
TEST(PerformerTest, Reset) {
  const float kNoteIndex = 1.0f;

  FakeInstrument instrument;
  Performer performer(&instrument);

  // Perform note on, then reset.
  performer.Perform(true, kNoteIndex, kNoteIntensity, 0);
  performer.Reset();

  std::vector<float> output(kNumSamples, 0.0f);
  performer.Process(0, kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

}  // namespace
}  // namespace barelyapi
