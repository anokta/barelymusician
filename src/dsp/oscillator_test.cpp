#include "dsp/oscillator.h"

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the oscillator generates sawtooth samples as expected.
TEST(OscillatorTest, Next) {
  constexpr int kCycleCount = 5;
  constexpr float kPitch = 0.0f;
  constexpr float kReferenceFrequency = 100.0f;
  constexpr float kSampleInterval = 0.0025f;
  constexpr float kShape = 1.0f;  // sawtooth
  constexpr float kSkew = 0.0f;

  Oscillator osc;
  osc.SetIncrement(kPitch, kReferenceFrequency, kSampleInterval);

  for (int cycle = 0; cycle < kCycleCount; ++cycle) {
    for (int i = 0; i < 4; ++i) {
      EXPECT_FLOAT_EQ(osc.GetOutput(kShape, kSkew),
                      (i < 2) ? static_cast<float>(i) * 0.5f : static_cast<float>(i - 4) * 0.5f);
      osc.Increment();
    }
  }
}

}  // namespace
}  // namespace barely
