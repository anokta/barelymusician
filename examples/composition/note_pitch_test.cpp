#include "examples/composition/note_pitch.h"

#include <iterator>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that expected note pitches are returned for a given arbitrary scale.
TEST(NotePitchTest, GetPitch) {
  const int kOctaveRange = 2;
  const std::vector<double> kScale(std::cbegin(kPitchMajorScale),
                                   std::cend(kPitchMajorScale));

  const int scale_length = static_cast<int>(kScale.size());
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int index = octave * scale_length + i;
      const double expected_pitch = static_cast<double>(octave) + kScale[i];
      EXPECT_DOUBLE_EQ(GetPitch(kScale, index), expected_pitch);
    }
  }
}

}  // namespace
}  // namespace barelyapi
