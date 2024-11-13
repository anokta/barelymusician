#include "dsp/oscillator.h"

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr double kCycleCount = 5;
  constexpr double kPitch = 0.0;
  constexpr double kReferenceFrequency = 100.0;
  constexpr double kSampleInterval = 0.0025;

  Oscillator oscillator;
  oscillator.SetIncrement(kPitch, kReferenceFrequency, kSampleInterval);

  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = -2; i < 2; ++i) {
      EXPECT_DOUBLE_EQ(oscillator.Next<OscillatorShape::kSaw>(), static_cast<double>(i) * 0.5);
    }
  }
}

}  // namespace
}  // namespace barely::internal
