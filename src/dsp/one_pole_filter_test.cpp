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
  OnePoleFilter low_pass_filter;

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(low_pass_filter.Next(kInput[i], kCoefficient, FilterType::kLowPass),
                     expected_output);
  }
}

// Tests that a low-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, LowPassAllPass) {
  OnePoleFilter low_pass_filter;

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(low_pass_filter.Next(input, 0.0, FilterType::kLowPass), input);
  }
}

// Tests that a high-pass filter generates the expected output when an arbitrary coefficient is set.
TEST(OnePoleFilterTest, HighPass) {
  OnePoleFilter high_pass_filter;

  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        kInput[i] - (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(high_pass_filter.Next(kInput[i], kCoefficient, FilterType::kHighPass),
                     expected_output);
  }
}

// Tests that a high-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(OnePoleFilterTest, HighPassAllPass) {
  OnePoleFilter high_pass_filter;

  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(high_pass_filter.Next(input, 1.0, FilterType::kHighPass), input);
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
