#include "dsp/one_pole_filter.h"

#include <array>
#include <cmath>

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Test input (single impulse).
constexpr int kInputLength = 5;
constexpr double kInput[kInputLength] = {1.0, 0.0, 0.0, 0.0, 0.0};

// Test coefficent.
constexpr double kCoefficient = 0.5;

// Tests that a low-pass filter generates the expected output when an arbitrary coefficient is set.
TEST(OnePoleFilterTest, LowPass) {
  double state = 0.0;

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(Filter<FilterType::kLowPass>(kInput[i], kCoefficient, state), expected_output);
  }
}

// Tests that a low-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, LowPassAllPass) {
  double state = 0.0;

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(Filter<FilterType::kLowPass>(input, 0.0, state), input);
  }
}

// Tests that a high-pass filter generates the expected output when an arbitrary coefficient is set.
TEST(OnePoleFilterTest, HighPass) {
  double state = 0.0;

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        kInput[i] - (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(Filter<FilterType::kHighPass>(kInput[i], kCoefficient, state),
                     expected_output);
  }
}

// Tests that a high-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, HighPassAllPass) {
  double state = 0.0;

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(Filter<FilterType::kHighPass>(input, 1.0, state), input);
  }
}

// Tests that the expected filter coefficients are generated for an arbitrary set of cutoff
// frequencies.
TEST(OnePoleFilterTest, GetFilterCoefficient) {
  constexpr double kEpsilon = 1e-2;
  constexpr int kSampleRate = 8000;

  constexpr int kCutoffCount = 5;
  constexpr std::array<double, kCutoffCount> kCutoffs = {
      0.0, 100.0, 500.0, 1000.0, 8000.0,
  };
  constexpr std::array<double, kCutoffCount> kExpectedCoefficients = {
      1.00, 0.92, 0.68, 0.46, 0.00,
  };

  for (int i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kSampleRate, kCutoffs[i]), kExpectedCoefficients[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely::internal
