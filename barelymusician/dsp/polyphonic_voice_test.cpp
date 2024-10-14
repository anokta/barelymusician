#include "barelymusician/dsp/polyphonic_voice.h"

#include "barelymusician/dsp/voice.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

constexpr int kVoiceCount = 4;
constexpr int kFrameRate = 1;

// Tests that playing a single voice produces the expected output.
TEST(PolyphonicVoiceTest, SingleVoice) {
  constexpr double kPitch = 0.25;
  constexpr double kGain = 0.5;

  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);

  polyphonic_voice.Update([&](Voice& voice) {
    voice.sample_player().SetData(&kPitch, kFrameRate, 1);
    voice.sample_player().SetLoop(true);
    voice.set_gain(kGain);
  });
  polyphonic_voice.Start(kPitch);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), kPitch * kGain);

  polyphonic_voice.Stop(kPitch);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);
}

// Tests that voice initialization callback produces the expected output.
TEST(PolyphonicVoiceTest, StartVoiceWithInit) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);

  constexpr double kOutput = 0.2;
  for (int i = 0; i < kVoiceCount; ++i) {
    const double pitch = static_cast<double>(i + 1);
    polyphonic_voice.Start(pitch, [&](Voice& voice) {
      voice.sample_player().SetData(&kOutput, kFrameRate, 1);
      voice.sample_player().SetLoop(true);
      voice.set_gain(pitch);
    });
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), pitch * kOutput);
    polyphonic_voice.Stop(pitch);
  }
}

// Tests that the number of voices that is played by the polyphonic voice is capped at maximum
// allowed number of voices.
TEST(PolyphonicVoiceTest, MaxVoices) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  polyphonic_voice.Resize(kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);

  constexpr double kOutput = 0.4;
  polyphonic_voice.Update([&](Voice& voice) {
    voice.sample_player().SetData(&kOutput, kFrameRate, 1);
    voice.sample_player().SetLoop(true);
  });

  for (int i = 0; i < kVoiceCount; ++i) {
    polyphonic_voice.Start(static_cast<double>(i), [&](Voice& voice) { voice.set_gain(1.0); });
    EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), static_cast<double>(i + 1) * kOutput);
  }

  polyphonic_voice.Start(kVoiceCount, [&](Voice& voice) { voice.set_gain(1.0); });
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), static_cast<double>(kVoiceCount) * kOutput);
}

// Tests that the polyphonic voice produces silence when there are no available voices set.
TEST(PolyphonicVoiceTest, NoVoice) {
  PolyphonicVoice polyphonic_voice(kFrameRate, kVoiceCount);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);

  polyphonic_voice.Start(0.0);
  EXPECT_DOUBLE_EQ(polyphonic_voice.Next(), 0.0);
}

}  // namespace
}  // namespace barely
