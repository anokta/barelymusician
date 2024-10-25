#include "barelymusician/dsp/oscillator.h"

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr double kCycleCount = 5;

  Oscillator oscillator(8);
  oscillator.SetShape(OscillatorShape::kSaw);
  oscillator.SetFrequency(2);
  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = -2; i < 2; ++i) {
      EXPECT_DOUBLE_EQ(oscillator.Next(), static_cast<double>(i) * 0.5);
    }
  }
}

// Tests that the oscillator clamps its maximum frequency as expected.
TEST(OscillatorTest, MaxFrequency) {
  constexpr double kCycleCount = 10;

  Oscillator oscillator(2);
  oscillator.SetShape(OscillatorShape::kSquare);
  oscillator.SetFrequency(1234);
  for (int i = 0; i < kCycleCount; ++i) {
    EXPECT_DOUBLE_EQ(oscillator.Next(), -1.0);
    EXPECT_DOUBLE_EQ(oscillator.Next(), 1.0);
  }
}

}  // namespace
}  // namespace barely::internal
