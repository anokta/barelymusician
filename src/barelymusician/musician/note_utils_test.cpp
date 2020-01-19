#include "barelymusician/musician/note_utils.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that notes are compared as expected according to their offset beats.
TEST(NoteUtilsTest, CompareNoteOffsetBeats) {
  const double kFirstOffsetBeats = 0.45;
  const double kSecondOffsetBeats = 0.7;

  Note first_note;
  first_note.offset_beats = kFirstOffsetBeats;
  Note second_note;
  second_note.offset_beats = kSecondOffsetBeats;

  EXPECT_TRUE(CompareNote(first_note, second_note));
  EXPECT_TRUE(CompareOffsetBeats(first_note, kSecondOffsetBeats));

  EXPECT_FALSE(CompareNote(second_note, first_note));
  EXPECT_FALSE(CompareOffsetBeats(second_note, kFirstOffsetBeats));
}

// Tests that note indices get quantized as expected given an arbitrary scale.
TEST(NoteUtilsTest, GetNoteIndex) {
  const int kOctaveRange = 2;

  const std::vector<float> kScale(std::cbegin(kMajorScale),
                                  std::cend(kMajorScale));
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

// Tests that the beat gets quantized as expected with respect to the given
// step.
TEST_P(GetBeatDurationTest, GetBeat) {
  const int kNumBeats = 4;
  const int num_steps_per_beat = GetParam();

  for (int beat = 0; beat < kNumBeats; ++beat) {
    for (int i = 0; i < num_steps_per_beat; ++i) {
      const double expected_beat =
          static_cast<double>(beat) +
          static_cast<double>(i) / static_cast<double>(num_steps_per_beat);
      EXPECT_DOUBLE_EQ(
          GetBeat(num_steps_per_beat * beat + i, num_steps_per_beat),
          expected_beat);
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
