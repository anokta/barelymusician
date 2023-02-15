#include "barelymusician/composition/duration.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

class PositionTest : public testing::TestWithParam<int> {};

// Tests that expected positions are returned with respect to the given steps.
TEST_P(PositionTest, GetPosition) {
  constexpr int kBeatCount = 4;
  const int step_count = GetParam();

  for (int beat = 0; beat < kBeatCount; ++beat) {
    for (int i = 0; i < step_count; ++i) {
      const double expected_position =
          static_cast<double>(beat) +
          static_cast<double>(i) / static_cast<double>(step_count);
      EXPECT_DOUBLE_EQ(GetPosition(step_count * beat + i, step_count),
                       expected_position);
    }
  }
}

// Tests that the position gets quantized as expected with respect to the given
// resolution.
TEST_P(PositionTest, QuantizePosition) {
  constexpr double kPosition = 0.99;
  const double resolution = 1.0 / static_cast<double>(GetParam());
  EXPECT_DOUBLE_EQ(QuantizePosition(kPosition, resolution), 1.0);
  EXPECT_DOUBLE_EQ(QuantizePosition(1.0 - kPosition, resolution), 0.0);
}

INSTANTIATE_TEST_SUITE_P(
    NoteDurationTest, PositionTest,
    testing::Values(kQuarterNotesPerBeat, kEighthNotesPerBeat,
                    kEighthTripletNotesPerBeat, kSixteenthNotesPerBeat,
                    kSixteenthTripletNotesPerBeat, kThirtySecondNotesPerBeat,
                    kThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barely
