#include "dsp/oscillator.h"

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr double kCycleCount = 5;
  constexpr double kIncrement = 0.25;

  double phase = 0.0;
  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = -2; i < 2; ++i) {
      EXPECT_DOUBLE_EQ(Oscillator<OscillatorShape::kSaw>(kIncrement, phase),
                       static_cast<double>(i) * 0.5);
    }
  }
}

}  // namespace
}  // namespace barely::internal
