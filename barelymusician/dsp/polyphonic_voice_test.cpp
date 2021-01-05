#include "barelymusician/dsp/polyphonic_voice.h"

#include <utility>

#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Number of voices for the polyphonic instrument.
constexpr int kNumVoices = 4;

// Default voice output value.
constexpr float kOutput = 1.0f;

// Default output channel.
constexpr int kChannel = 0;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  FakeVoice() : active_(false), output_(0.0f) {}

  // Implements |UnitGenerator|.
  float Next(int) override { return active_ ? output_ : 0.0f; }

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
  const float kPitch = 0.0f;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  polyphonic_voice.Start(kPitch);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), kOutput);

  polyphonic_voice.Stop(kPitch);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  for (int i = 0; i < kNumVoices; ++i) {
    const float pitch = static_cast<float>(i + 1);
    polyphonic_voice.Start(
        pitch, [pitch](FakeVoice* voice) { voice->SetOutput(pitch); });
    EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), pitch);
    polyphonic_voice.Stop(pitch);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is
// capped at maximum allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  float previous_output = 0.0f;
  for (int i = 0; i < kNumVoices; ++i) {
    polyphonic_voice.Start(static_cast<float>(i));
    const float output = polyphonic_voice.Next(kChannel);
    EXPECT_FLOAT_EQ(output - previous_output, kOutput);
    previous_output = output;
  }

  polyphonic_voice.Start(static_cast<float>(kNumVoices));
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), previous_output);
}

// Tests that the polyphonic voice produces silence when there are no available
// voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  polyphonic_voice.Start(0);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  const float kUpdatedOutput = 4.0f * kOutput;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kNumVoices);
  EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  for (int i = 0; i < kNumVoices; ++i) {
    const float pitch = static_cast<float>(i);
    polyphonic_voice.Start(pitch);
    EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), kOutput);
    polyphonic_voice.Stop(pitch);
  }

  polyphonic_voice.Update(
      [kUpdatedOutput](FakeVoice* voice) { voice->SetOutput(kUpdatedOutput); });

  for (int i = 0; i < kNumVoices; ++i) {
    const float pitch = static_cast<float>(i);
    polyphonic_voice.Start(pitch);
    EXPECT_FLOAT_EQ(polyphonic_voice.Next(kChannel), kUpdatedOutput);
    polyphonic_voice.Stop(pitch);
  }
}

}  // namespace
}  // namespace barelyapi
