#include "barelymusician/dsp/polyphonic_voice.h"

#include <utility>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Number of voices for the polyphonic instrument.
constexpr Integer kVoiceCount = 4;

// Default voice output value.
constexpr Real kOutput = 1.0;

// Default output channel.
constexpr Integer kChannel = 0;

// Fake voice that produces constant output for testing.
class FakeVoice : public Voice {
 public:
  // Implements `UnitGenerator`.
  Real Next(Integer /*channel*/) noexcept override {
    return active_ ? output_ : 0.0;
  }

  // Implements `Voice`.
  [[nodiscard]] bool IsActive() const noexcept override { return active_; }
  void Start() noexcept override { active_ = true; }
  void Stop() noexcept override { active_ = false; }

  // Sets voice output.
  void SetOutput(Real output) noexcept { output_ = output; }

 private:
  bool active_ = false;
  Real output_ = 0.0;
};

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  const Real kPitch = 0.0;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(kPitch);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);

  polyphonic_voice.Stop(kPitch);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  for (Integer i = 0; i < kVoiceCount; ++i) {
    const Real pitch = static_cast<Real>(i + 1);
    polyphonic_voice.Start(
        pitch, [pitch](FakeVoice* voice) { voice->SetOutput(pitch); });
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), pitch);
    polyphonic_voice.Stop(pitch);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is
// capped at maximum allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  Real previous_output = 0.0;
  for (Integer i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(static_cast<Real>(i));
    const Real output = polyphonic_voice.Next(kChannel);
    EXPECT_DOUBLE_EQ(output - previous_output, kOutput);
    previous_output = output;
  }

  polyphonic_voice.Start(static_cast<Real>(kVoiceCount));
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), previous_output);
}

// Tests that the polyphonic voice produces silence when there are no available
// voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(0);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  const Real kUpdatedOutput = 4.0 * kOutput;

  FakeVoice base_voice;
  base_voice.SetOutput(kOutput);

  PolyphonicVoice<FakeVoice> polyphonic_voice(std::move(base_voice));
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  for (Integer i = 0; i < kVoiceCount; ++i) {
    const Real pitch = static_cast<Real>(i);
    polyphonic_voice.Start(pitch);
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);
    polyphonic_voice.Stop(pitch);
  }

  polyphonic_voice.Update(
      [kUpdatedOutput](FakeVoice* voice) { voice->SetOutput(kUpdatedOutput); });

  for (Integer i = 0; i < kVoiceCount; ++i) {
    const Real pitch = static_cast<Real>(i);
    polyphonic_voice.Start(pitch);
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kUpdatedOutput);
    polyphonic_voice.Stop(pitch);
  }
}

}  // namespace
}  // namespace barely
