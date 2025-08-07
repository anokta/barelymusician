#include "dsp/biquad_filter.h"

#include <barelymusician.h>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 4;
constexpr float kInput[kInputLength] = {1.0f, -1.0f, 1.0f, 0.5f};

constexpr float kSampleRate = 8000.0f;
constexpr float kSampleInterval = 1.0f / kSampleRate;

constexpr float kFilterQ = 1.0f;
constexpr float kEpsilon = 1e-5f;

// Tests that a low-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(BiquadFilterTest, LowPassAllPass) {
  const auto coeffs =
      GetFilterCoefficients(kSampleInterval, FilterType::kLowPass, kSampleRate, kFilterQ);

  BiquadFilter filter;
  for (const float input : kInput) {
    EXPECT_NEAR(filter.Next(input, coeffs), input, kEpsilon);
  }
}

// Tests that a high-pass filter does not alter the input when the coefficient is set to all-pass.
TEST(BiquadFilterTest, HighPassAllPass) {
  const auto coeffs = GetFilterCoefficients(kSampleInterval, FilterType::kHighPass, 0.0f, kFilterQ);

  BiquadFilter filter;
  for (const float input : kInput) {
    EXPECT_NEAR(filter.Next(input, coeffs), input, kEpsilon);
  }
}

}  // namespace
}  // namespace barely
