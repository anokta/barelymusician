#include "barelymusician/composition/intensity.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected note intensities are returned for given midi note velocities.
TEST(PitchTest, PitchFromMidi) {
  constexpr int kValueCount = 5;
  constexpr std::array<int, kValueCount> kMidis = {
      0, 32, 64, 95, 127,
  };
  constexpr std::array<double, kValueCount> kIntensities = {
      0.0, 0.25, 0.5, 0.75, 1.0,
  };

  constexpr double kIntensityEpsilon = 0.5 / kMaxMidiVelocity;

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(MidiFromIntensity(kIntensities[i]), kMidis[i]);
    EXPECT_NEAR(IntensityFromMidi(kMidis[i]), kIntensities[i], kIntensityEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(MidiFromIntensity(IntensityFromMidi(kMidis[i])), kMidis[i]);
    EXPECT_NEAR(IntensityFromMidi(MidiFromIntensity(kIntensities[i])), kIntensities[i],
                kIntensityEpsilon);
  }
}

}  // namespace
}  // namespace barely
