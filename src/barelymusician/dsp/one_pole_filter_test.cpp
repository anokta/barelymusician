#include "barelymusician/dsp/one_pole_filter.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Test input (single impulse).
const int kInputLength = 5;
const float kInput[kInputLength] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

// Test coefficent.
const float kCoefficient = 0.5f;

// Tolerated error margin.
const float kEpsilon = 1e-6f;

// Tests that a low-pass filter generates the expected output when an arbitrary
// coefficient is set.
TEST(OnePoleFilterTest, LowPass) {
  OnePoleFilter low_pass_filter;
  low_pass_filter.SetType(FilterType::kLowPass);
  low_pass_filter.SetCoefficient(kCoefficient);

  for (int i = 0; i < kInputLength; ++i) {
    const float expected_output =
        (1.0f - kCoefficient) * std::pow(kCoefficient, static_cast<float>(i));
    EXPECT_FLOAT_EQ(low_pass_filter.Next(kInput[i]), expected_output);
  }
}

// Tests that a low-pass filter does not alter the input when the coefficient is
// set to all-pass.
TEST(OnePoleFilterTest, LowPassAllPass) {
  OnePoleFilter low_pass_filter;
  low_pass_filter.SetType(FilterType::kLowPass);
  low_pass_filter.SetCoefficient(0.0f);

  for (int i = 0; i < kInputLength; ++i) {
    EXPECT_FLOAT_EQ(low_pass_filter.Next(kInput[i]), kInput[i]);
  }
}

// Tests that a high-pass filter generates the expected output when an arbitrary
// coefficient is set.
TEST(OnePoleFilterTest, HighPass) {
  OnePoleFilter high_pass_filter;
  high_pass_filter.SetType(FilterType::kHighPass);
  high_pass_filter.SetCoefficient(kCoefficient);

  for (int i = 0; i < kInputLength; ++i) {
    const float expected_output =
        kInput[i] -
        (1.0f - kCoefficient) * std::pow(kCoefficient, static_cast<float>(i));
    EXPECT_FLOAT_EQ(high_pass_filter.Next(kInput[i]), expected_output);
  }
}

// Tests that a high-pass filter does not alter the input when the coefficient
// is set to all-pass.
TEST(OnePoleFilterTest, HighPassAllPass) {
  OnePoleFilter high_pass_filter;
  high_pass_filter.SetType(FilterType::kHighPass);
  high_pass_filter.SetCoefficient(1.0f);

  for (int i = 0; i < kInputLength; ++i) {
    EXPECT_FLOAT_EQ(high_pass_filter.Next(kInput[i]), kInput[i]);
  }
}

}  // namespace
}  // namespace barelyapi
