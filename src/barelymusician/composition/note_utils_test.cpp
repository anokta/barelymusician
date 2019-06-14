#include "barelymusician/composition/note_utils.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that note indices get quantized as expected given an arbitrary scale.
TEST(NoteUtilsTest, GetNoteIndex) {
  const int kOctaveRange = 2;

  const std::vector<float> kScale(std::begin(kMajorScale),
                                  std::end(kMajorScale));
  const int scale_length = static_cast<int>(kScale.size());

  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const float scale_index = static_cast<float>(octave * scale_length + i);
      const float expected_note_index =
          static_cast<float>(octave * kNumSemitones) + kScale[i];
      EXPECT_FLOAT_EQ(GetNoteIndex(kScale, scale_index), expected_note_index);
    }
  }
}

class GetBeatDurationTest : public testing::TestWithParam<int> {};

// Tests that the beat duration gets quantized as expected with respect to the
// given number of notes.
TEST_P(GetBeatDurationTest, GetBeatDuration) {
  const int kNumBeats = 4;
  const int num_notes_per_beat = GetParam();

  for (int beat = 0; beat < kNumBeats; ++beat) {
    for (int i = 0; i < num_notes_per_beat; ++i) {
      const float expected_duration_beats =
          static_cast<float>(beat) +
          static_cast<float>(i) / static_cast<float>(num_notes_per_beat);
      EXPECT_FLOAT_EQ(
          GetBeatDuration(num_notes_per_beat * beat + i, num_notes_per_beat),
          expected_duration_beats);
    }
  }
}

INSTANTIATE_TEST_CASE_P(NoteUtilsTest, GetBeatDurationTest,
                        testing::Values(kNumQuarterNotesPerBeat,
                                        kNumEighthNotesPerBeat,
                                        kNumEighthTripletNotesPerBeat,
                                        kNumSixteenthNotesPerBeat,
                                        kNumSixteenthTripletNotesPerBeat,
                                        kNumThirtySecondNotesPerBeat,
                                        kNumThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barelyapi
