#include "dsp/distortion.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 5;
constexpr double kInput[kInputLength] = {0.2, -0.4, 0.6, -0.8, 1.0};

constexpr double kDrive = 1.0;

TEST(DistortionTest, FullMix) {
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(Distortion(input, 1.0, kDrive), std::tanh(input));
  }
}

TEST(DistortionTest, Bypass) {
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(Distortion(input, 0.0, kDrive), input);
  }
}

}  // namespace
}  // namespace barely
