#include "barelymusician/dsp/polyphonic_voice.h"

#include <utility>

#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Number of voices for the polyphonic instrument.
constexpr int kVoiceCount = 4;

// Default voice output value.
// constexpr double kOutput = 1.0;

// Default output channel.
constexpr int kChannel = 0;

constexpr int kFrameRate = 1;

// TODO(#139): Reenable the sample checks with explicit samples using `sample_player().SetData`.

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  constexpr double kPitch = 0.25;

  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(kPitch);
  // EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);

  polyphonic_voice.Stop(kPitch);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  // for (int i = 0; i < kVoiceCount; ++i) {
  //   const double pitch = static_cast<double>(i + 1);
  //   polyphonic_voice.Start(pitch, [pitch](Voice* voice) { voice->SetOutput(pitch); });
  //   EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), pitch);
  //   polyphonic_voice.Stop(pitch);
  // }
}

// Tests that the number of voices that is played by the polyphonic voice is capped at maximum
// allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  // double previous_output = 0.0;
  // for (int i = 0; i < kVoiceCount; ++i) {
  //   polyphonic_voice.Start(static_cast<double>(i));
  //   const double output = polyphonic_voice.Next(kChannel);
  //   EXPECT_DOUBLE_EQ(output - previous_output, kOutput);
  //   previous_output = output;
  // }

  // polyphonic_voice.Start(kVoiceCount);
  // EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), previous_output);
}

// Tests that the polyphonic voice produces silence when there are no available voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  polyphonic_voice.Start(0.0);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);
}

// Tests that the voice update callback updates all the voices as expected.
TEST(PolyphonicVoiceTest, Update) {
  // constexpr double kUpdatedOutput = 4.0 * kOutput;

  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), 0.0);

  // for (int i = 0; i < kVoiceCount; ++i) {
  //   polyphonic_voice.Start(static_cast<double>(i));
  //   EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kOutput);
  //   polyphonic_voice.Stop(static_cast<double>(i));
  // }

  polyphonic_voice.Update([](Voice& voice) { voice.set_gain(1.0); });

  // for (int i = 0; i < kVoiceCount; ++i) {
  //   polyphonic_voice.Start(static_cast<double>(i));
  //   EXPECT_DOUBLE_EQ(polyphonic_voice.Next(kChannel), kUpdatedOutput);
  //   polyphonic_voice.Stop(static_cast<double>(i));
  // }
}

}  // namespace
}  // namespace barely
