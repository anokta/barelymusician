#include "barelymusician/dsp/oscillator.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr int kCycleCount = 5;

  Oscillator oscillator(8);
  oscillator.SetType(OscillatorType::kSaw);
  oscillator.SetFrequency(2);
  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = -2; i < 2; ++i) {
      EXPECT_EQ(oscillator.Next(), static_cast<float>(i) * 0.5f);
    }
  }
}

// Tests that the oscillator clamps its maximum frequency as expected.
TEST(OscillatorTest, MaxFrequency) {
  constexpr int kCycleCount = 10;

  Oscillator oscillator(2);
  oscillator.SetType(OscillatorType::kSquare);
  oscillator.SetFrequency(1234);
  for (int i = 0; i < kCycleCount; ++i) {
    EXPECT_EQ(oscillator.Next(), -1.0f);
    EXPECT_EQ(oscillator.Next(), 1.0f);
  }
}

}  // namespace
}  // namespace barely
