#include "dsp/distortion.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 5;
constexpr float kInput[kInputLength] = {0.2f, -0.4f, 0.6f, -0.8f, 1.0f};

constexpr float kDrive = 1.0f;

TEST(DistortionTest, FullMix) {
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(Distortion(input, 1.0f, kDrive), std::tanh(input));
  }
}

TEST(DistortionTest, Bypass) {
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(Distortion(input, 0.0f, kDrive), input);
  }
}

}  // namespace
}  // namespace barely
