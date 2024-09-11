#include "barelymusician/composition/midi.h"

#include <array>

#include "barelymusician/composition/scale.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the expected note intensities are returned for the given midi note velocities.
TEST(MidiTest, IntensityMidiVelocityConversion) {
  constexpr int kValueCount = 5;
  constexpr std::array<int, kValueCount> kMidiVelocities = {
      0, 32, 64, 95, 127,
  };
  constexpr std::array<double, kValueCount> kIntensities = {
      0.0, 0.25, 0.5, 0.75, 1.0,
  };

  constexpr double kIntensityEpsilon = 0.5 / 127.0;

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_DOUBLE_EQ(MidiVelocityFromIntensity(kIntensities[i]), kMidiVelocities[i]);
    EXPECT_NEAR(IntensityFromMidiVelocity(kMidiVelocities[i]), kIntensities[i], kIntensityEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(MidiVelocityFromIntensity(IntensityFromMidiVelocity(kMidiVelocities[i])),
                     kMidiVelocities[i]);
    EXPECT_NEAR(IntensityFromMidiVelocity(MidiVelocityFromIntensity(kIntensities[i])),
                kIntensities[i], kIntensityEpsilon);
  }
}

// Tests that converting arbitrary midi note numbers returns the expected frequencies.
TEST(MidiTest, FrequencyFromMidiNumber) {
  constexpr double kEpsilon = 1e-2;

  constexpr int kMidiNumberCount = 4;
  constexpr std::array<int, kMidiNumberCount> kMidiNumbers = {
      21,
      60,
      69,
      93,
  };
  constexpr std::array<double, kMidiNumberCount> kFrequencies = {
      27.50,
      261.62,
      440.00,
      1760.00,
  };

  for (int i = 0; i < kMidiNumberCount; ++i) {
    EXPECT_NEAR(FrequencyFromMidiNumber(kMidiNumbers[i]), kFrequencies[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
