#include "common/time.h"

#include <array>
#include <cstdint>

#include "gtest/gtest-param-test.h"
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

// Tests that frames and seconds are converted to each other as expected.
TEST(TimeTest, FramesSecondsConversion) {
  constexpr int kSampleRate = 8000;

  constexpr int kValueCount = 4;
  constexpr std::array<int64_t, kValueCount> kFrames = {0, 800, 4000, 32000};
  constexpr std::array<double, kValueCount> kSeconds = {0.0, 0.1, 0.5, 4.0};

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(FramesToSeconds(kSampleRate, kFrames[i]), kSeconds[i]);
    EXPECT_EQ(SecondsToFrames(kSampleRate, kSeconds[i]), kFrames[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(FramesToSeconds(kSampleRate, SecondsToFrames(kSampleRate, kSeconds[i])),
                     kSeconds[i]);
    EXPECT_EQ(SecondsToFrames(kSampleRate, FramesToSeconds(kSampleRate, kFrames[i])), kFrames[i]);
  }
}

// Common subdivisions of a beat in relation to quarter note beat duration.
constexpr int kQuarterNotesPerBeat = 1;
constexpr int kEighthNotesPerBeat = 2;
constexpr int kEighthTripletNotesPerBeat = 3;
constexpr int kSixteenthNotesPerBeat = 4;
constexpr int kSixteenthTripletNotesPerBeat = 6;
constexpr int kThirtySecondNotesPerBeat = 8;
constexpr int kThirtySecondTripletNotesPerBeat = 12;

class QuantizationTestWithParam : public testing::TestWithParam<int> {};

// Tests that the position gets quantized as expected with respect to the given resolution.
TEST_P(QuantizationTestWithParam, Quantize) {
  constexpr double kPosition = 0.99;

  EXPECT_DOUBLE_EQ(Quantize(kPosition, GetParam(), 1.0), 1.0);
  EXPECT_DOUBLE_EQ(Quantize(1.0 - kPosition, GetParam(), 1.0), 0.0);
}

INSTANTIATE_TEST_SUITE_P(QuantizationTest, QuantizationTestWithParam,
                         testing::Values(kQuarterNotesPerBeat, kEighthNotesPerBeat,
                                         kEighthTripletNotesPerBeat, kSixteenthNotesPerBeat,
                                         kSixteenthTripletNotesPerBeat, kThirtySecondNotesPerBeat,
                                         kThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barely
