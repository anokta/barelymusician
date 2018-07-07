#include "barelymusician/instrument/polyphonic_instrument.h"

#include "barelymusician/instrument/voice.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

#ifndef UNUSED_PARAM(param)
#define UNUSED_PARAM(param) param
#endif  // UNUSED_PARAM(param)

// Number of voices for the polyphonic instrument.
constexpr int kNumVoices = 4;

// Voice intensity.
constexpr float kIntensity = 1.0f;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  FakeVoice(float output) : output_(output), intensity_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return intensity_ * output_; }
  void Reset() override { intensity_ = 0.0f; }

  // Implements |Voice|.
  bool IsActive() const override { return intensity_ > 0.0f; }
  void Start(float index, float intensity) override {
    UNUSED_PARAM(index);
    intensity_ = intensity;
  }
  void Stop() override { intensity_ = 0.0f; }

 private:
  float output_;
  float intensity_;
};

// Fake polyphonic instrument for testing.
class FakePolyphonicInstrument : public PolyphonicInstrument {
 public:
  FakePolyphonicInstrument(int num_voices) {
    voices_.reserve(num_voices);
    for (int i = 0; i < num_voices; ++i) {
      voices_.emplace_back(new FakeVoice(static_cast<float>(i + 1)));
    }
    Reset();
  }

  // Implements |Instrument|.
  const void* GetParam(int id) const override {
    UNUSED_PARAM(id);
    return nullptr;
  }
  void SetParam(int id, void* value) override {
    UNUSED_PARAM(id);
    UNUSED_PARAM(value);
  }
};

// Tests that toggling a note of an instrument on/off produces expected output.
TEST(PolyphonicInstrumentTest, SingleNoteOnOff) {
  const float kNoteIndex = 0.0f;

  FakePolyphonicInstrument instrument(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, instrument.Next());

  instrument.NoteOn(kNoteIndex, kIntensity);
  EXPECT_NE(0.0f, instrument.Next());

  instrument.NoteOff(kNoteIndex);
  EXPECT_FLOAT_EQ(0.0f, instrument.Next());
}

// Tests that number of voices that produces the instrument output is capped at
// maximum allowed number of voices.
TEST(PolyphonicInstrumentTest, MaxVoices) {
  FakePolyphonicInstrument instrument(kNumVoices);

  float previous_output = 0.0f;
  for (int i = 0; i < kNumVoices; ++i) {
    instrument.NoteOn(static_cast<float>(i), kIntensity);
    const float output = instrument.Next();
    EXPECT_GT(output, previous_output);
    previous_output = output;
  }

  instrument.NoteOn(static_cast<float>(kNumVoices), kIntensity);
  EXPECT_FLOAT_EQ(previous_output, instrument.Next());
}

// Tests that resetting the instrument resets all the active voices properly.
TEST(PolyphonicInstrumentTest, Reset) {
  FakePolyphonicInstrument instrument(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, instrument.Next());

  for (int i = 0; i < kNumVoices; ++i) {
    instrument.NoteOn(static_cast<float>(i), kIntensity);
    EXPECT_NE(0.0f, instrument.Next());
  }

  instrument.Reset();
  EXPECT_FLOAT_EQ(0.0f, instrument.Next());
}

}  // namespace
}  // namespace barelyapi
