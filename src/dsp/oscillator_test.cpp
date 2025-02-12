#include "dsp/oscillator.h"

#include "barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr int kCycleCount = 5;
  constexpr float kPitch = 0.0f;
  constexpr float kReferenceFrequency = 100.0f;
  constexpr float kSampleInterval = 0.0025f;
  constexpr float kPulseWidth = 0.5f;

  Oscillator oscillator;
  oscillator.SetIncrement(kPitch, kReferenceFrequency, kSampleInterval);

  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = -2; i < 2; ++i) {
      EXPECT_FLOAT_EQ(oscillator.Next<OscillatorShape::kSaw>(kPulseWidth),
                      static_cast<float>(i) * 0.5f);
    }
  }
}

}  // namespace
}  // namespace barely
