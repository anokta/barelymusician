#include "barelymusician/composition/scale.h"

#include <array>
#include <cmath>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected notes are returned for a given arbitrary scale.
TEST(ScaleTest, GetNote) {
  constexpr int kNoteCount = 5;
  const std::array<double, kNoteCount> kRatios = {1.25, 1.5, 1.75, 2.0, 3.0};
  constexpr double kRootNote = 100.0;

  const Scale scale(ScaleDefinition{kRatios}, kRootNote);

  constexpr int kOctaveRange = 2;
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < kNoteCount; ++i) {
      const int degree = octave * kNoteCount + i;
      const double expected_note =
          kRootNote * std::pow(kRatios[kNoteCount - 1], octave) * (i > 0 ? kRatios[i - 1] : 1.0);
      EXPECT_DOUBLE_EQ(scale.GetNote(degree), expected_note);
      EXPECT_DOUBLE_EQ(scale.GetNote(octave, i), expected_note);
    }
  }
}

}  // namespace
}  // namespace barely
