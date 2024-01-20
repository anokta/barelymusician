#include "barelymusician/composition/intensity.h"

#include <array>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected note intensities are returned for given midi note velocities.
TEST(IntensityTest, IntensityFromMidi) {
  constexpr int kValueCount = 5;
  constexpr std::array<int, kValueCount> kMidis = {
      0, 31, 63, 95, 127,
  };
  constexpr std::array<float, kValueCount> kIntensities = {
      0, 0.25f, 0.5f, 0.75f, 1,
  };

  constexpr float kEpsilon = 1.0f / static_cast<float>(kMaxMidiVelocity);

  for (int i = 0; i < kValueCount; ++i) {
    EXPECT_EQ(MidiFromIntensity(kIntensities[i]), kMidis[i]);
    EXPECT_NEAR(IntensityFromMidi(kMidis[i]), kIntensities[i], kEpsilon);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(MidiFromIntensity(IntensityFromMidi(kMidis[i])), kMidis[i]);
    EXPECT_NEAR(IntensityFromMidi(MidiFromIntensity(kIntensities[i])), kIntensities[i], kEpsilon);
  }
}

}  // namespace
}  // namespace barely
