#include "dsp/one_pole_filter.h"

#include <array>
#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input (single impulse).
constexpr int kInputLength = 5;
constexpr double kInput[kInputLength] = {1.0, 0.0, 0.0, 0.0, 0.0};

constexpr double kCoefficient = 0.5;

TEST(OnePoleFilterTest, Lpf) {
  OnePoleFilter filter;
  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(filter.Next<FilterType::kLowPass>(kInput[i], kCoefficient), expected_output);
  }
}

TEST(OnePoleFilterTest, LpfAllPass) {
  OnePoleFilter filter;
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(filter.Next<FilterType::kLowPass>(input, 0.0), input);
  }
}

TEST(OnePoleFilterTest, Hpf) {
  OnePoleFilter filter;
  for (int i = 0; i < kInputLength; ++i) {
    const double expected_output =
        kInput[i] - (1.0 - kCoefficient) * std::pow(kCoefficient, static_cast<double>(i));
    EXPECT_DOUBLE_EQ(filter.Next<FilterType::kHighPass>(kInput[i], kCoefficient), expected_output);
  }
}

TEST(OnePoleFilterTest, HpfAllPass) {
  OnePoleFilter filter;
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(filter.Next<FilterType::kHighPass>(input, 1.0), input);
  }
}

TEST(OnePoleFilterTest, GetFilterCoeff) {
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
    EXPECT_NEAR(GetFilterCoeff(kSampleRate, kCutoffs[i]), kExpectedCoefficients[i], kEpsilon) << i;
  }
}

}  // namespace
}  // namespace barely
