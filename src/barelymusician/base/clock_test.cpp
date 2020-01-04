#include "barelymusician/base/clock.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
const int kSampleRate = 48000;

// Clock tempo.
const float kTempo = 120.0f;

// Tests that the clock triggers the registered beat callback as expected.
TEST(ClockTest, SetBeatCallback) {
  const int kNumSeconds = 8;

  Clock clock(kSampleRate);
  clock.SetTempo(kTempo);

  int expected_beat = 0;
  const auto beat_callback = [&expected_beat](int beat, int) {
    EXPECT_EQ(beat, expected_beat);
    ++expected_beat;
  };
  clock.SetBeatCallback(beat_callback);

  for (int i = 0; i < kNumSeconds; ++i) {
    clock.Update(kSampleRate);
  }
  EXPECT_NE(expected_beat, 0);
}

// Tests that the clock position gets set as expected.
TEST(ClockTest, SetPosition) {
  const float kPosition = 2.4f;

  Clock clock(kSampleRate);
  clock.SetTempo(kTempo);

  EXPECT_FLOAT_EQ(clock.GetPosition(), 0.0f);

  clock.SetPosition(kPosition);
  EXPECT_FLOAT_EQ(clock.GetPosition(), kPosition);
}

// Tests that the clock tempo gets set as expected.
TEST(ClockTest, SetTempo) {
  Clock clock(kSampleRate);
  EXPECT_FLOAT_EQ(clock.GetTempo(), 0.0f);
  EXPECT_EQ(clock.GetNumSamplesPerBeat(), 0);

  clock.SetTempo(kTempo);
  EXPECT_FLOAT_EQ(clock.GetTempo(), kTempo);
  EXPECT_EQ(clock.GetNumSamplesPerBeat(),
            static_cast<int>(static_cast<float>(kSampleRate) *
                             kSecondsFromMinutes / kTempo));
}

// Tests that the clock gets updated as expected.
TEST(ClockTest, Update) {
  Clock clock(kSampleRate);
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);
  EXPECT_FLOAT_EQ(clock.GetPosition(), 0.0f);

  clock.Update(kSampleRate);
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);
  EXPECT_FLOAT_EQ(clock.GetPosition(), 0.0f);

  clock.SetTempo(kTempo);

  clock.Update(kSampleRate);
  EXPECT_GE(clock.GetBeat(), 0);
  EXPECT_GE(clock.GetLeftoverSamples(), 0);
  EXPECT_FLOAT_EQ(clock.GetPosition(), kTempo / kSecondsFromMinutes);
}

}  // namespace
}  // namespace barelyapi
