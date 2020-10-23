#include "barelymusician/engine/note_utils.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that note indices get quantized as expected given an arbitrary scale.
TEST(NoteUtilsTest, GetRawNoteIndex) {
  const int kOctaveRange = 2;

  const std::vector<float> kScale(std::cbegin(kMajorScale),
                                  std::cend(kMajorScale));
  const int scale_length = static_cast<int>(kScale.size());

  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int scale_index = octave * scale_length + i;
      const float expected_note_index =
          static_cast<float>(octave * kNumSemitones) + kScale[i];
      EXPECT_FLOAT_EQ(
          GetRawNoteIndex(kScale, QuantizedNoteIndex{0.0f, scale_index}),
          expected_note_index);
    }
  }
}

class GetRawPositionTest : public testing::TestWithParam<int> {};

// Tests that the beat gets quantized as expected with respect to the given
// step.
TEST_P(GetRawPositionTest, GetRawPosition) {
  const int kNumBeats = 4;
  const int num_steps = GetParam();

  for (int beat = 0; beat < kNumBeats; ++beat) {
    for (int i = 0; i < num_steps; ++i) {
      const double expected_position =
          static_cast<double>(beat) +
          static_cast<double>(i) / static_cast<double>(num_steps);
      EXPECT_DOUBLE_EQ(
          GetRawPosition(QuantizedPosition{num_steps * beat + i, num_steps}),
          expected_position);
    }
  }
}

INSTANTIATE_TEST_CASE_P(NoteUtilsTest, GetRawPositionTest,
                        testing::Values(kNumQuarterNotesPerBeat,
                                        kNumEighthNotesPerBeat,
                                        kNumEighthTripletNotesPerBeat,
                                        kNumSixteenthNotesPerBeat,
                                        kNumSixteenthTripletNotesPerBeat,
                                        kNumThirtySecondNotesPerBeat,
                                        kNumThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barelyapi
