#include "barelymusician/composition/scale.h"

#include <utility>
#include <vector>

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that note indices are transformed correctly given an arbitrary scale.
TEST(ScaleTest, GetNoteIndex) {
  const std::vector<float> kMajorScale = {0.0f, 2.0f, 4.0f, 5.0f,
                                          7.0f, 9.0f, 11.0f};
  const int kOctaveRange = 2;

  Scale scale(kMajorScale);

  const int scale_length = scale.GetLength();
  EXPECT_EQ(kMajorScale.size(), scale_length);

  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const float relative_note_index =
          static_cast<float>(octave * scale_length + i);
      const float expected_note_index =
          static_cast<float>(octave * kNumSemitones) + kMajorScale[i];
      EXPECT_FLOAT_EQ(expected_note_index,
                      scale.GetNoteIndex(relative_note_index));
    }
  }
}

}  // namespace
}  // namespace barelyapi
