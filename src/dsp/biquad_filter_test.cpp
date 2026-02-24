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

TEST(BiquadFilterTest, LowPassAllPass) {
  const auto coeffs =
      // NOLINTNEXTLINE(readability-suspicious-call-argument)
      GetFilterCoefficients(kSampleInterval, FilterType::kLpf, kSampleRate, kFilterQ);

  BiquadFilter filter;
  for (const float input : kInput) {
    EXPECT_NEAR(filter.Next(input, coeffs), input, kEpsilon);
  }
}

TEST(BiquadFilterTest, HighPassAllPass) {
  const auto coeffs = GetFilterCoefficients(kSampleInterval, FilterType::kHpf, 0.0f, kFilterQ);

  BiquadFilter filter;
  for (const float input : kInput) {
    EXPECT_NEAR(filter.Next(input, coeffs), input, kEpsilon);
  }
}

}  // namespace
}  // namespace barely
