#include "barelymusician/composition/score.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

const int kSectionType = 4;

// Tests that the score returns its written notes as expected.
TEST(ScoreTest, GetNotes) {
  const int kNumBars = 3;
  const int kNumBeats = 2;

  Score score;

  for (int bar = 0; bar < kNumBars; ++bar) {
    for (int beat = 0; beat < kNumBeats; ++beat) {
      // Add a single note per beat.
      auto* notes = score.GetNotes(kSectionType, bar, beat);
      ASSERT_NE(notes, nullptr);
      EXPECT_TRUE(notes->empty());

      Note note;
      note.index = static_cast<float>(bar * kNumBeats + beat);
      notes->push_back(note);
    }
  }

  for (int bar = 0; bar < kNumBars; ++bar) {
    for (int beat = 0; beat < kNumBeats; ++beat) {
      // Verify that each beat note still exists in the correct location.
      const auto* notes = score.GetNotes(kSectionType, bar, beat);
      ASSERT_NE(notes, nullptr);
      ASSERT_EQ(notes->size(), 1);

      const Note& note = notes->front();
      EXPECT_EQ(note.index, static_cast<float>(bar * kNumBeats + beat));
    }
  }
}

// Tests that the score clears its written notes as expected.
TEST(ScoreTest, Clear) {
  const int kBar = 1;
  const int kBeat = 3;

  Score score;

  auto* notes = score.GetNotes(kSectionType, kBar, kBeat);
  ASSERT_NE(notes, nullptr);
  EXPECT_TRUE(notes->empty());

  // Add note.
  notes->emplace_back();

  notes = score.GetNotes(kSectionType, kBar, kBeat);
  ASSERT_NE(notes, nullptr);
  EXPECT_FALSE(notes->empty());

  // Clear score.
  score.Clear();

  notes = score.GetNotes(kSectionType, kBar, kBeat);
  ASSERT_NE(notes, nullptr);
  EXPECT_TRUE(notes->empty());
}

}  // namespace
}  // namespace barelyapi
