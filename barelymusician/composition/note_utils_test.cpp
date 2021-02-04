#include "barelymusician/composition/note_utils.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that expected note pitches are returned given an arbitrary scale.
TEST(NoteUtilsTest, GetPitch) {
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
