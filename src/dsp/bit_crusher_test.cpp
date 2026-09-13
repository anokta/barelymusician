#include "dsp/bit_crusher.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 5;
constexpr double kInput[kInputLength] = {0.2, -0.4, 0.6, -0.8, 1.0};

TEST(BitCrusherTest, BitDepth) {
  constexpr double kIncrement = 1.0;

  BitCrusher bit_crusher;
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(bit_crusher.Next(input, 0.0, kIncrement), input);              // bypass
    EXPECT_DOUBLE_EQ(bit_crusher.Next(input, 1.0, kIncrement), std::round(input));  // 1-bit
  }
}

TEST(BitCrusherTest, SampleRate) {
  constexpr double kRange = 0.0;

  BitCrusher bit_crusher;

  // Bypass.
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(bit_crusher.Next(input, kRange, 1.0), input);
  }
  bit_crusher.Reset();

  // Hold every other sample.
  for (int i = 0; i < kInputLength; ++i) {
    EXPECT_DOUBLE_EQ(bit_crusher.Next(kInput[i], kRange, 0.5),
                     (i > 0) ? kInput[2 * ((i - 1) / 2) + 1] : 0.0);
  }
  bit_crusher.Reset();

  // Hold forever.
  for (const double input : kInput) {
    EXPECT_DOUBLE_EQ(bit_crusher.Next(input, kRange, 0.0), 0.0);
  }
}

}  // namespace
}  // namespace barely
