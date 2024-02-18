#include "barelymusician/composition/duration.h"

#include "gtest/gtest-param-test.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

class DurationTestWithParam : public testing::TestWithParam<double> {};

// Tests that the position gets quantized as expected with respect to the given resolution.
TEST_P(DurationTestWithParam, QuantizePosition) {
  constexpr double kPosition = 0.99;
  const double resolution = 1.0 / static_cast<double>(GetParam());
  EXPECT_DOUBLE_EQ(QuantizePosition(kPosition, resolution), 1.0);
  EXPECT_DOUBLE_EQ(QuantizePosition(1.0 - kPosition, resolution), 0.0);
}

INSTANTIATE_TEST_SUITE_P(DurationTest, DurationTestWithParam,
                         testing::Values(kQuarterNotesPerBeat, kEighthNotesPerBeat,
                                         kEighthTripletNotesPerBeat, kSixteenthNotesPerBeat,
                                         kSixteenthTripletNotesPerBeat, kThirtySecondNotesPerBeat,
                                         kThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barely
