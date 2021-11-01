#include "barelymusician/composition/note_pitch.h"

#include <iterator>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that expected note pitches are returned given an arbitrary scale.
TEST(NotePitchTest, GetPitch) {
  const int kOctaveRange = 2;
  const std::vector<float> kScale(std::cbegin(kPitchMajorScale),
                                  std::cend(kPitchMajorScale));

  const int scale_length = static_cast<int>(kScale.size());
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int scale_index = octave * scale_length + i;
      const float expected_pitch = static_cast<float>(octave) + kScale[i];
      EXPECT_FLOAT_EQ(GetPitch(kScale, scale_index), expected_pitch);
    }
  }
}

}  // namespace
}  // namespace barelyapi
