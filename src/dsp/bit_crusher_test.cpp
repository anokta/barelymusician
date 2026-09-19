#include "dsp/bit_crusher.h"

#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Test input.
constexpr int kInputLength = 5;
constexpr float kInput[kInputLength] = {0.2f, -0.4f, 0.6f, -0.8f, 1.0f};

TEST(BitCrusherTest, BitDepth) {
  constexpr float kIncrement = 1.0f;

  BitCrusher bit_crusher;
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(bit_crusher.Next(input, 0.0f, kIncrement), input);              // bypass
    EXPECT_FLOAT_EQ(bit_crusher.Next(input, 1.0f, kIncrement), std::round(input));  // 1-bit
  }
}

TEST(BitCrusherTest, SampleRate) {
  constexpr float kRange = 0.0f;

  BitCrusher bit_crusher;

  // Bypass.
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(bit_crusher.Next(input, kRange, 1.0f), input);
  }
  bit_crusher.Reset();

  // Hold every other sample.
  for (int i = 0; i < kInputLength; ++i) {
    EXPECT_FLOAT_EQ(bit_crusher.Next(kInput[i], kRange, 0.5f),
                    (i > 0) ? kInput[2 * ((i - 1) / 2) + 1] : 0.0f);
  }
  bit_crusher.Reset();

  // Hold forever.
  for (const float input : kInput) {
    EXPECT_FLOAT_EQ(bit_crusher.Next(input, kRange, 0.0f), 0.0f);
  }
}

}  // namespace
}  // namespace barely
