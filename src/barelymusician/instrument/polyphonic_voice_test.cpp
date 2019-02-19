#include "barelymusician/instrument/polyphonic_voice.h"

#include "barelymusician/instrument/voice.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Number of voices for the polyphonic instrument.
constexpr int kNumVoices = 4;

// Default voice output value.
constexpr float kOutput = 1.0f;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  FakeVoice() : active_(false), output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next() override { return active_ ? output_ : 0.0f; }
  void Reset() override { active_ = false; }

  // Implements |Voice|.
  bool IsActive() const override { return active_; }
  void Start() override { active_ = true; }
  void Stop() override { active_ = false; }

  // Sets voice output.
  void SetOutput(float output) { output_ = output; }

 private:
  bool active_;
  float output_;
};

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  const float kNoteIndex = 0.0f;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  polyphonic_voice.Start(kNoteIndex, nullptr);
  EXPECT_FLOAT_EQ(kOutput, polyphonic_voice.Next());

  polyphonic_voice.Stop(kNoteIndex, nullptr);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  for (int i = 0; i < kNumVoices; ++i) {
    const float index = static_cast<float>(i + 1);
    polyphonic_voice.Start(
        index, [index](FakeVoice* voice) { voice->SetOutput(index); });
    const float output = polyphonic_voice.Next();
    EXPECT_FLOAT_EQ(index, polyphonic_voice.Next());
    polyphonic_voice.Stop(index, nullptr);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is
// capped at maximum allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  float previous_output = 0.0f;
  for (int i = 0; i < kNumVoices; ++i) {
    const float index = static_cast<float>(i);
    polyphonic_voice.Start(static_cast<float>(i), nullptr);
    const float output = polyphonic_voice.Next();
    EXPECT_FLOAT_EQ(kOutput, output - previous_output);
    previous_output = output;
  }

  polyphonic_voice.Start(static_cast<float>(kNumVoices), nullptr);
  EXPECT_FLOAT_EQ(previous_output, polyphonic_voice.Next());
}

// Tests that the polyphonic voice produces silence when there are no available
// voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  polyphonic_voice.Start(0, nullptr);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());
}

// Tests that resetting the polyphonic voice resets all the active voices
// properly.
TEST(PolyphonicVoiceTest, Reset) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  for (int i = 0; i < kNumVoices; ++i) {
    polyphonic_voice.Start(static_cast<float>(i), nullptr);
    EXPECT_NE(0.0f, polyphonic_voice.Next());
  }

  polyphonic_voice.Reset();
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  const float kUpdatedOutput = 4.0f * kOutput;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(base_voice);
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(0.0f, polyphonic_voice.Next());

  for (int i = 0; i < kNumVoices; ++i) {
    const float index = static_cast<float>(i);
    polyphonic_voice.Start(index, nullptr);
    EXPECT_FLOAT_EQ(kOutput, polyphonic_voice.Next());
    polyphonic_voice.Stop(index, nullptr);
  }

  polyphonic_voice.Update(
      [kUpdatedOutput](FakeVoice* voice) { voice->SetOutput(kUpdatedOutput); });

  for (int i = 0; i < kNumVoices; ++i) {
    const float index = static_cast<float>(i);
    polyphonic_voice.Start(index, nullptr);
    EXPECT_FLOAT_EQ(kUpdatedOutput, polyphonic_voice.Next());
    polyphonic_voice.Stop(index, nullptr);
  }
}

}  // namespace
}  // namespace barelyapi
