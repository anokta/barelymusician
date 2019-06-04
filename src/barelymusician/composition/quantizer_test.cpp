#include "barelymusician/composition/quantizer.h"

#include "barelymusician/base/constants.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

class QuantizerTest : public testing::TestWithParam<int> {};

// Tests that the quantizer returns the expected duration in beats in relation
// to the given number of notes.
TEST_P(QuantizerTest, GetDurationBeats) {
  const int kNumBeats = 4;

  const int num_notes_per_beat = GetParam();
  Quantizer quantizer(num_notes_per_beat);

  for (int beat = 0; beat < kNumBeats; ++beat) {
    for (int i = 0; i < num_notes_per_beat; ++i) {
      const float expected_duration_beats =
          static_cast<float>(beat) +
          static_cast<float>(i) / static_cast<float>(num_notes_per_beat);
      EXPECT_FLOAT_EQ(quantizer.GetDurationBeats(num_notes_per_beat * beat + i),
                      expected_duration_beats);
    }
  }
}

INSTANTIATE_TEST_CASE_P(NumNotesPerBeat, QuantizerTest,
                        testing::Values(kNumQuarterNotesPerBeat,
                                        kNumEighthNotesPerBeat,
                                        kNumEighthTripletNotesPerBeat,
                                        kNumSixteenthNotesPerBeat,
                                        kNumSixteenthTripletNotesPerBeat,
                                        kNumThirtySecondNotesPerBeat,
                                        kNumThirtySecondTripletNotesPerBeat));

}  // namespace
}  // namespace barelyapi
