#include "barelymusician/composition/scale.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected pitches are returned for a given arbitrary scale.
TEST(ScaleTest, GetPitch) {
  constexpr int kPitchCount = 5;
  const std::array<double, kPitchCount> kPitches = {0.0, 0.2, 0.35, 0.5, 0.95};
  constexpr double kRootPitch = 1.75;
  constexpr int kMode = 1;

  const ScaleDefinition scale = {kPitches, kRootPitch, kMode};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int degree = octave * kPitchCount + i;
      // NOLINTNEXTLINE(bugprone-integer-division)
      const double expected_pitch = kRootPitch + octave + (i + kMode) / kPitchCount +
                                    kPitches[(i + kMode) % kPitchCount] - kPitches[kMode];
      EXPECT_EQ(scale.GetPitch(degree), expected_pitch) << degree;
    }
  }
}

}  // namespace
}  // namespace barely
