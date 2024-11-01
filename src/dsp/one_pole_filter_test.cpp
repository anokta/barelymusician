#include "dsp/one_pole_filter.h"

#include <array>
#include <cmath>

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
  OnePoleFilter low_pass_filter;
  low_pass_filter.SetType(FilterType::kLowPass);
  low_pass_filter.SetCoefficient(kCoefficient);

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(low_pass_filter.Next(kInput[i]), expected_output);
  }
}

// Tests that a low-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, LowPassAllPass) {
  OnePoleFilter low_pass_filter;
  low_pass_filter.SetType(FilterType::kLowPass);
  low_pass_filter.SetCoefficient(0.0);

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(low_pass_filter.Next(input), input);
  }
}

// Tests that a high-pass filter generates the expected output when an arbitrary coefficient is set.
TEST(OnePoleFilterTest, HighPass) {
  OnePoleFilter high_pass_filter;
  high_pass_filter.SetType(FilterType::kHighPass);
  high_pass_filter.SetCoefficient(kCoefficient);

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        kInput[i] - (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(high_pass_filter.Next(kInput[i]), expected_output);
  }
}

// Tests that a high-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, HighPassAllPass) {
  OnePoleFilter high_pass_filter;
  high_pass_filter.SetType(FilterType::kHighPass);
  high_pass_filter.SetCoefficient(1.0);

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(high_pass_filter.Next(input), input);
  }
}

// Tests that the expected filter coefficients are generated for an arbitrary set of cutoff
// frequencies.
TEST(OnePoleFilterTest, GetFilterCoefficient) {
  constexpr double kEpsilon = 1e-2;
  constexpr int kFrameRate = 8000;

  constexpr int kCutoffCount = 5;
  constexpr std::array<double, kCutoffCount> kCutoffs = {
      0.0, 100.0, 500.0, 1000.0, 8000.0,
  };
  constexpr std::array<double, kCutoffCount> kExpectedCoefficients = {
      1.00, 0.92, 0.68, 0.46, 0.00,
  };

  for (int i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kFrameRate, kCutoffs[i]), kExpectedCoefficients[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely::internal
