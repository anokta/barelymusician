#include "common/time.h"

#include <array>
#include <cstdint>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that beats and seconds are converted to each other as expected.
TEST(TimeTest, BeatsSecondsConversion) {
  constexpr double kTempo = 120.0;

  constexpr int kValueCount = 5;
  constexpr std::array<double, kValueCount> kBeats = {0.0, 1.0, 5.0, -4.0, -24.6};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.5, 2.5, -2.0, -12.3};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(BeatsToSeconds(kTempo, kBeats[i]), kSeconds[i]);
    EXPECT_DOUBLE_EQ(SecondsToBeats(kTempo, kSeconds[i]), kBeats[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(BeatsToSeconds(kTempo, SecondsToBeats(kTempo, kSeconds[i])), kSeconds[i]);
    EXPECT_DOUBLE_EQ(SecondsToBeats(kTempo, BeatsToSeconds(kTempo, kBeats[i])), kBeats[i]);
  }
}

// Tests that beats and ticks are converted to each other as expected.
TEST(TimeTest, BeatsTicksConversion) {
  constexpr int kValueCount = 6;
  constexpr int kTickCount = static_cast<int>(kBeatsToTicks);
  constexpr std::array<double, kValueCount> kBeats = {
      0.0, 1.0, 5.0, -4.0, 0.25, -0.125,
  };
  constexpr std::array<int64_t, kValueCount> kTicks = {
      0, kTickCount, 5 * kTickCount, -4 * kTickCount, kTickCount / 4, -kTickCount / 8,
  };

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(BeatsToTicks(kBeats[i]), kTicks[i]);
    EXPECT_DOUBLE_EQ(TicksToBeats(kTicks[i]), kBeats[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(BeatsToTicks(TicksToBeats(kTicks[i])), kTicks[i]);
    EXPECT_DOUBLE_EQ(TicksToBeats(BeatsToTicks(kBeats[i])), kBeats[i]);
  }
}

// Tests that samples and seconds are converted to each other as expected.
TEST(TimeTest, SamplesSecondsConversion) {
  constexpr int kSampleRate = 8000;

  constexpr int kValueCount = 4;
  constexpr std::array<int64_t, kValueCount> kSamples = {0, 800, 4000, 32000};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.1, 0.5, 4.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(SamplesToSeconds(kSampleRate, kSamples[i]), kSeconds[i]);
    EXPECT_EQ(SecondsToSamples(kSampleRate, kSeconds[i]), kSamples[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(SamplesToSeconds(kSampleRate, SecondsToSamples(kSampleRate, kSeconds[i])),
                     kSeconds[i]);
    EXPECT_EQ(SecondsToSamples(kSampleRate, SamplesToSeconds(kSampleRate, kSamples[i])),
              kSamples[i]);
  }
}

}  // namespace
}  // namespace barely
