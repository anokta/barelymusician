#include "dsp/one_pole_filter.h"

#include <barelymusician.h>

#include <array>
#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input (single impulse).
constexpr int kInputLength = 5;
constexpr float kInput[kInputLength] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

constexpr float kCoefficient = 0.5f;

TEST(OnePoleFilterTest, Lpf) {
  OnePoleFilter filter;
  for (int i = 0; i < kInputLength; ++i) {
    const float expected_output =
        (1.0f - kCoefficient) * std::pow(kCoefficient, static_cast<float>(i));
    EXPECT_FLOAT_EQ(filter.Next<FilterType::kLpf>(kInput[i], kCoefficient), expected_output);
  }
}

TEST(OnePoleFilterTest, LpfAllPass) {
  OnePoleFilter filter;
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(filter.Next<FilterType::kLpf>(input, 0.0f), input);
  }
}

TEST(OnePoleFilterTest, Hpf) {
  OnePoleFilter filter;
  for (int i = 0; i < kInputLength; ++i) {
    const float expected_output =
        kInput[i] - (1.0f - kCoefficient) * std::pow(kCoefficient, static_cast<float>(i));
    EXPECT_FLOAT_EQ(filter.Next<FilterType::kHpf>(kInput[i], kCoefficient), expected_output);
  }
}

TEST(OnePoleFilterTest, HpfAllPass) {
  OnePoleFilter filter;
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(filter.Next<FilterType::kHpf>(input, 1.0f), input);
  }
}

TEST(OnePoleFilterTest, GetFilterCoefficient) {
  constexpr float kEpsilon = 1e-2f;
  constexpr int kSampleRate = 8000;

  constexpr int kCutoffCount = 5;
  constexpr std::array<float, kCutoffCount> kCutoffs = {
      0.0f, 100.0f, 500.0f, 1000.0f, 8000.0f,
  };
  constexpr std::array<float, kCutoffCount> kExpectedCoefficients = {
      1.00f, 0.92f, 0.68f, 0.46f, 0.00f,
  };

  for (int i = 0; i < kCutoffCount; ++i) {
    EXPECT_NEAR(GetFilterCoefficient(kSampleRate, kCutoffs[i]), kExpectedCoefficients[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
