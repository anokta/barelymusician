#include "dsp/distortion.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 5;
constexpr float kInput[kInputLength] = {0.2f, -0.4f, 0.6f, -0.8f, 1.0f};

constexpr float kDrive = 1.0f;

// Tests that distortion is applied as expected with full amount.
TEST(DistortionTest, FullAmount) {
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(Distortion(input, 1.0f, kDrive), std::tanh(input));
  }
}

// Tests that distortion is bypassed as expected with zero amount.
TEST(DistortionTest, Bypass) {
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(Distortion(input, 0.0f, kDrive), input);
  }
}

}  // namespace
}  // namespace barely
