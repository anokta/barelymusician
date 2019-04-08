#include "barelymusician/instrument/instrument_processor.h"

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
  void SetFloatParam(ParamId id, float value) override {
    DLOG(INFO) << "SetFloatParam(" << id << ", " << value << ")";
    output_ = value;
  }

 private:
  float output_;
};

// Tests that playing a single note produces the expected output.
TEST(InstrumentProcessorTest, PlaySingleNote) {
  const float kNoteIndex = 32.0f;

  FakeInstrument instrument;
  InstrumentProcessor instrument_processor(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Play note.
  instrument_processor.NoteOn(0, kNoteIndex, kNoteIntensity);

  output.assign(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kNoteIndex, output[i]);
  }

  // Stop note.
  instrument_processor.NoteOff(0, kNoteIndex);

  output.assign(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that playing multiple notes produces the expected output.
TEST(InstrumentProcessorTest, PlayMultipleNotes) {
  FakeInstrument instrument;
  InstrumentProcessor instrument_processor(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (const auto& sample : output) {
    EXPECT_FLOAT_EQ(0.0f, sample);
  }

  // Play new note per each sample in the buffer.
  for (int i = 0; i < kNumSamples; ++i) {
    instrument_processor.NoteOn(i, static_cast<float>(i), kNoteIntensity);
  }

  output.assign(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(static_cast<float>(i), output[i]);
  }

  // Stop all notes.
  for (int i = 0; i < kNumSamples; ++i) {
    instrument_processor.NoteOff(0, static_cast<float>(i));
  }

  output.assign(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

// Tests that updating a float parameter produces the expected output.
TEST(InstrumentProcessorTest, SetFloatParam) {
  const ParamId kFloatParamId = 0;
  const float kFloatParamValue = 0.5f;

  FakeInstrument instrument;
  InstrumentProcessor instrument_processor(&instrument);

  std::vector<float> output(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }

  // Update float parameter.
  instrument_processor.SetFloatParam(0, kFloatParamId, kFloatParamValue);

  output.assign(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(kFloatParamValue, output[i]);
  }
}

// Tests that resetting the processor clears out the output buffer as expected.
TEST(InstrumentProcessorTest, Reset) {
  const float kNoteIndex = 1.0f;

  FakeInstrument instrument;
  InstrumentProcessor instrument_processor(&instrument);

  // Play note, then reset.
  instrument_processor.NoteOn(0, kNoteIndex, kNoteIntensity);
  instrument_processor.Reset();

  std::vector<float> output(kNumSamples, 0.0f);
  instrument_processor.Process(kNumSamples, output.data());
  for (int i = 0; i < kNumSamples; ++i) {
    EXPECT_FLOAT_EQ(0.0f, output[i]);
  }
}

}  // namespace
}  // namespace barelyapi
