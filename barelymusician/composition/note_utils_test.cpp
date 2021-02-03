#include "barelymusician/composition/note_utils.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that expected note numbers are returned given an arbitrary scale.
TEST(NoteUtilsTest, GetNoteNumber) {
  const int kOctaveRange = 2;

  const std::vector<int> kScale(std::cbegin(kMajorScale),
                                std::cend(kMajorScale));
  const int scale_length = static_cast<int>(kScale.size());

  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int scale_index = octave * scale_length + i;
      const int expected_note_number = octave * kNumSemitones + kScale[i];
      EXPECT_EQ(GetNoteNumber(kScale, scale_index), expected_note_number)
          << scale_index;
    }
  }
}

// Tests that expected pitches are returned with respect to the given note
// numbers.
TEST(NoteUtilsTest, GetPitch) {
  const float kEpsilon = 1e-2f;

  const int kNumNoteNumbers = 4;
  const int kNoteNumbers[kNumNoteNumbers] = {21, 60, 69, 93};
  const float kPitches[kNumNoteNumbers] = {-4.00f, -0.75f, 0.00f, 2.00f};

  for (int i = 0; i < kNumNoteNumbers; ++i) {
    EXPECT_NEAR(GetPitch(kNoteNumbers[i]), kPitches[i], kEpsilon);
  }
}

class PositionTest : public testing::TestWithParam<int> {};

// Tests that expected positions are returned with respect to the given steps.
TEST_P(PositionTest, GetPosition) {
  const int kNumBeats = 4;
  const int num_steps = GetParam();

  for (int beat = 0; beat < kNumBeats; ++beat) {
    for (int i = 0; i < num_steps; ++i) {
      const double expected_position =
          static_cast<double>(beat) +
          static_cast<double>(i) / static_cast<double>(num_steps);
      EXPECT_DOUBLE_EQ(GetPosition(num_steps * beat + i, num_steps),
                       expected_position);
    }
  }
}

// Tests that the position gets quantized as expected with respect to the given
// resolution.
TEST_P(PositionTest, QuantizePosition) {
  const double kPosition = 0.99;
  const double resolution = 1.0 / static_cast<double>(GetParam());
  EXPECT_DOUBLE_EQ(QuantizePosition(kPosition, resolution), 1.0);
  EXPECT_DOUBLE_EQ(QuantizePosition(1.0 - kPosition, resolution), 0.0);
}

INSTANTIATE_TEST_SUITE_P(NoteUtilsTest, PositionTest,
                         testing::Values(kNumQuarterNotesPerBeat,
                                         kNumEighthNotesPerBeat,
                                         kNumEighthTripletNotesPerBeat,
                                         kNumSixteenthNotesPerBeat,
                                         kNumSixteenthTripletNotesPerBeat,
                                         kNumThirtySecondNotesPerBeat,
                                         kNumThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barelyapi
