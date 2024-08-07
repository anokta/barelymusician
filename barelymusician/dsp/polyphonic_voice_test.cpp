#include "barelymusician/dsp/polyphonic_voice.h"

#include <utility>

#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Number of voices for the polyphonic instrument.
constexpr int kVoiceCount = 4;

// Default voice output value.
constexpr double kOutput = 1.0;

// Default output channel.
constexpr int kChannel = 0;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  // Implements `UnitGenerator`.
  double Next(int /*channel*/) noexcept override { return active_ ? output_ : 0.0; }

  // Implements `Voice`.
  [[nodiscard]] bool IsActive() const noexcept override { return active_; }
  void Start() noexcept override { active_ = true; }
  void Stop() noexcept override { active_ = false; }

  // Sets voice output.
  void SetOutput(double output) noexcept { output_ = output; }

 private:
  bool active_ = false;
  double output_ = 0.0;
};

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  constexpr int kNoteId = 1;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(kNoteId);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);

  polyphonic_voice.Stop(kNoteId);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  for (int i = 0; i < kVoiceCount; ++i) {
    double pitch = static_cast<double>(i + 1);
    polyphonic_voice.Start(i + 1, [pitch](FakeVoice* voice) { voice->SetOutput(pitch); });
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), pitch);
    polyphonic_voice.Stop(i + 1);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is capped at maximum
// allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  double previous_output = 0.0;
  for (int i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(i);
    const double output = polyphonic_voice.Next(kChannel);
    EXPECT_DOUBLE_EQ(output - previous_output, kOutput);
    previous_output = output;
  }

  polyphonic_voice.Start(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), previous_output);
}

// Tests that the polyphonic voice produces silence when there are no available voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(0);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  constexpr double kUpdatedOutput = 4.0 * kOutput;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice), kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  for (int i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(i);
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);
    polyphonic_voice.Stop(i);
  }

  polyphonic_voice.Update([kUpdatedOutput](FakeVoice* voice) { voice->SetOutput(kUpdatedOutput); });

  for (int i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(i);
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kUpdatedOutput);
    polyphonic_voice.Stop(i);
  }
}

}  // namespace
}  // namespace barely
