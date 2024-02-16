#include "barelymusician/composition/midi.h"

#include <array>

#include "barelymusician/composition/pitch.h"
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

  constexpr double kIntensityEpsilon = 0.5 / kMaxMidiVelocity;

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

// Tests that the expected note pitches are returned for the given midi note numbers.
TEST(MidiTest, MidiNumberPitchConversion) {
  constexpr std::array<int, static_cast<int>(kSemitoneCount)> kMidiNumbers = {
      kMidiNumberA4,     kMidiNumberA4 + 1, kMidiNumberA4 + 2,  kMidiNumberA4 + 3,
      kMidiNumberA4 + 4, kMidiNumberA4 + 5, kMidiNumberA4 + 6,  kMidiNumberA4 + 7,
      kMidiNumberA4 + 8, kMidiNumberA4 + 9, kMidiNumberA4 + 10, kMidiNumberA4 + 11,
  };
  constexpr std::array<double, static_cast<int>(kSemitoneCount)> kPitches = {
      kPitchA4,      kPitchAsharp4, kPitchB4, kPitchC5,      kPitchCsharp5, kPitchD5,
      kPitchDsharp5, kPitchE5,      kPitchF5, kPitchFsharp5, kPitchG5,      kPitchGsharp5,
  };

  for (int i = 0; i < static_cast<int>(kSemitoneCount); ++i) {
    EXPECT_DOUBLE_EQ(MidiNumberFromPitch(kPitches[i]), kMidiNumbers[i]);
    EXPECT_DOUBLE_EQ(PitchFromMidiNumber(kMidiNumbers[i]), kPitches[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(MidiNumberFromPitch(PitchFromMidiNumber(kMidiNumbers[i])), kMidiNumbers[i]);
    EXPECT_DOUBLE_EQ(PitchFromMidiNumber(MidiNumberFromPitch(kPitches[i])), kPitches[i]);
  }
}

}  // namespace
}  // namespace barely
