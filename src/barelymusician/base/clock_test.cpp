#include "barelymusician/base/clock.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Sampling rate.
constexpr int kSampleRate = 48000;

// Clock tempo.
constexpr float kTempo = 120.0f;

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

// Tests that the clock gets reset as expected.
TEST(ClockTest, Reset) {
  Clock clock(kSampleRate);
  clock.SetTempo(kTempo);
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);

  clock.Update(kSampleRate);
  EXPECT_EQ(clock.GetBeat(), static_cast<int>(kTempo / kSecondsFromMinutes));
  EXPECT_GE(clock.GetLeftoverSamples(), 0);

  clock.Reset();
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);
}

// Tests that the clock gets updated as expected.
TEST(ClockTest, Update) {
  Clock clock(kSampleRate);
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);

  clock.Update(kSampleRate);
  EXPECT_EQ(clock.GetBeat(), 0);
  EXPECT_EQ(clock.GetLeftoverSamples(), 0);

  clock.SetTempo(kTempo);

  clock.Update(kSampleRate);
  EXPECT_GE(clock.GetBeat(), static_cast<int>(kTempo / kSecondsFromMinutes));
  EXPECT_GE(clock.GetLeftoverSamples(), 0);
}

}  // namespace
}  // namespace barelyapi
