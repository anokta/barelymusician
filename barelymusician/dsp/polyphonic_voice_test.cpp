#include "barelymusician/dsp/polyphonic_voice.h"

#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Number of voices for the polyphonic instrument.
constexpr int kVoiceCount = 4;

// Default voice output value.
constexpr float kOutput = 1.0f;

// Default output channel.
constexpr int kChannel = 0;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  // Implements `UnitGenerator`.
  float Next(int /*channel*/) noexcept override { return active_ ? output_ : 0.0f; }

  // Implements `Voice`.
  [[nodiscard]] bool IsActive() const noexcept override { return active_; }
  void Start() noexcept override { active_ = true; }
  void Stop() noexcept override { active_ = false; }

  // Sets voice output.
  void SetOutput(float output) noexcept { output_ = static_cast<float>(output); }

 private:
  bool active_ = false;
  float output_ = 0.0f;
};

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  constexpr Rational kPitch = 0;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  polyphonic_voice.Start(kPitch);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), kOutput);

  polyphonic_voice.Stop(kPitch);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  for (int i = 0; i < kVoiceCount; ++i) {
    const Rational pitch = i + 1;
    polyphonic_voice.Start(
        pitch, [pitch](FakeVoice* voice) { voice->SetOutput(static_cast<float>(pitch)); });
    EXPECT_EQ(polyphonic_voice.Next(kChannel), static_cast<float>(pitch));
    polyphonic_voice.Stop(pitch);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is capped at maximum
// allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  float previous_output = 0.0f;
  for (int i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(i);
    const float output = polyphonic_voice.Next(kChannel);
    EXPECT_EQ(output - previous_output, kOutput);
    previous_output = output;
  }

  polyphonic_voice.Start(kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), previous_output);
}

// Tests that the polyphonic voice produces silence when there are no available voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  polyphonic_voice.Start(0);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  constexpr float kUpdatedOutput = 4.0f * kOutput;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_EQ(polyphonic_voice.Next(kChannel), 0.0f);

  for (int i = 0; i < kVoiceCount; ++i) {
    const Rational pitch = i;
    polyphonic_voice.Start(pitch);
    EXPECT_EQ(polyphonic_voice.Next(kChannel), kOutput);
    polyphonic_voice.Stop(pitch);
  }

  polyphonic_voice.Update([kUpdatedOutput](FakeVoice* voice) { voice->SetOutput(kUpdatedOutput); });

  for (int i = 0; i < kVoiceCount; ++i) {
    const Rational pitch = i;
    polyphonic_voice.Start(pitch);
    EXPECT_EQ(polyphonic_voice.Next(kChannel), kUpdatedOutput);
    polyphonic_voice.Stop(pitch);
  }
}

}  // namespace
}  // namespace barely
