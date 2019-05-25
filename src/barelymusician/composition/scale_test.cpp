#include "barelymusician/composition/scale.h"

#include <utility>
#include <vector>

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that note indices are transformed correctly given an arbitrary scale.
TEST(ScaleTest, GetNoteIndex) {
  const int kOctaveRange = 2;

  const std::vector<float> kScale(std::begin(kMajorScale),
                                  std::end(kMajorScale));
  Scale scale(kScale);

  const int scale_length = scale.GetLength();
  EXPECT_EQ(kScale.size(), scale_length);

  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const float scale_index = static_cast<float>(octave * scale_length + i);
      const float expected_note_index =
          static_cast<float>(octave * kNumSemitones) + kScale[i];
      EXPECT_FLOAT_EQ(expected_note_index, scale.GetNoteIndex(scale_index));
    }
  }
}

}  // namespace
}  // namespace barelyapi
