#include "barelymusician/instrument/instrument_utils.h"

#include "barelymusician/instrument/instrument.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tolerated error margin.
const float kEpsilon = 1e-1f;

// Test instrument that sets test output according to note off/on calls.
class TestInstrument : public Instrument {
 public:
  TestInstrument() : output_(0.0f) {}

  // Implements |Instrument|.
  void AllNotesOff() override {}
  void NoteOff(float) override { output_ = 0.0f; }
  void NoteOn(float index, float intensity) override {
    output_ = index * intensity;
  }
  void Process(float*, int, int) override {}

  float GetOutput() const { return output_; }

 private:
  float output_;
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

TEST(InstrumentUtilsTest, ProcessMessage) {
  const float kNoteIndex = 60.0f;
  const float kNoteIntensity = 0.5f;
  const int kTimestamp = 32;

  TestInstrument instrument;
  EXPECT_FLOAT_EQ(instrument.GetOutput(), 0.0f);

  // Process note on message.
  ProcessMessage(BuildNoteOnMessage(kNoteIndex, kNoteIntensity, kTimestamp),
                 &instrument);
  EXPECT_FLOAT_EQ(instrument.GetOutput(), kNoteIndex * kNoteIntensity);

  // Process note off message.
  ProcessMessage(BuildNoteOffMessage(kNoteIndex, kTimestamp), &instrument);
  EXPECT_FLOAT_EQ(instrument.GetOutput(), 0.0f);
}

}  // namespace
}  // namespace barelyapi
