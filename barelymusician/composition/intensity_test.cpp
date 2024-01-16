#include "barelymusician/composition/intensity.h"

#include <array>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected note intensities are returned for given midi note velocities.
TEST(PitchTest, PitchFromMidi) {
  constexpr int kValueCount = 5;
  constexpr std::array<int, kValueCount> kMidis = {
      0, 1, 64, 95, 127,
  };
  constexpr std::array<Rational, kValueCount> kIntensities = {
      0, Rational(1, 127), Rational(64, 127), Rational(95, 127), 1,
  };

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(MidiFromIntensity(kIntensities[i]), kMidis[i]);
    EXPECT_EQ(IntensityFromMidi(kMidis[i]), kIntensities[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(MidiFromIntensity(IntensityFromMidi(kMidis[i])), kMidis[i]);
    EXPECT_EQ(IntensityFromMidi(MidiFromIntensity(kIntensities[i])), kIntensities[i]);
  }
}

}  // namespace
}  // namespace barely
