#include "barelymusician/composition/scale.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected pitches are returned for a given arbitrary scale.
TEST(ScaleTest, GetPitch) {
  constexpr int kPitchCount = 5;
  const std::array<int, kPitchCount> kPitches = {2, 4, 10, 20, 25};
  constexpr int kRootPitch = 50;
  constexpr int kMode = 0;

  const ScaleDefinition scale = {kPitches, kRootPitch, kMode};

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kPitchCount; ++i) {
      const int degree = octave * kPitchCount + i;
      const int expected_pitch =
          kRootPitch + kPitches[kPitchCount - 1] * octave + (i > 0 ? kPitches[i - 1] : 0);
      EXPECT_EQ(scale.GetPitch(degree), expected_pitch);
    }
  }
}

}  // namespace
}  // namespace barely
