#include "barelymusician/musician/score.h"

#include "barelymusician/musician/note.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

constexpr int kBeat = 5;
constexpr float kNoteIndex = 60.0f;
constexpr float kNoteIntensity = 1.0f;
constexpr double kNoteDurationBeats = 0.5;

// Tests that the score returns an added single note as expected.
TEST(ScoreTest, SingleNote) {
  const double kNoteOffsetBeats = 0.25;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add note.
  score.AddNote(kBeat, {kNoteIndex, kNoteIntensity, kNoteOffsetBeats,
                        kNoteDurationBeats});
  EXPECT_FALSE(score.Empty());
  EXPECT_FALSE(score.Empty(kBeat));

  // Get note.
  const auto* notes = score.GetNotes(kBeat);
  ASSERT_NE(notes, nullptr);
  ASSERT_EQ(notes->size(), 1);

  const Note& note = notes->front();
  EXPECT_FLOAT_EQ(note.index, kNoteIndex);
  EXPECT_FLOAT_EQ(note.intensity, kNoteIntensity);
  EXPECT_DOUBLE_EQ(note.offset_beats, kNoteOffsetBeats);
  EXPECT_DOUBLE_EQ(note.duration_beats, kNoteDurationBeats);

  // Remove note.
  score.Clear(kBeat);
  EXPECT_TRUE(score.Empty());
  EXPECT_TRUE(score.Empty(kBeat));
}

// Tests that the score returns added notes as expected.
TEST(ScoreTest, MultipleNotes) {
  const int kNumNotes = 5;
  const double kNoteOffsetBeats = 0.75;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add |kNumNotes| notes, each note with |kNoteOffsetBeats| from each beat.
  for (int i = 0; i < kNumNotes; ++i) {
    score.AddNote(i, {static_cast<float>(i), kNoteIntensity, kNoteOffsetBeats,
                      kNoteDurationBeats});
    EXPECT_FALSE(score.Empty(i));
  }
  EXPECT_FALSE(score.Empty());

  // Get one note at a time.
  for (int i = 0; i < kNumNotes; ++i) {
    const auto* notes = score.GetNotes(i);
    ASSERT_NE(notes, nullptr);
    ASSERT_EQ(notes->size(), 1);

    const Note& note = notes->front();
    EXPECT_FLOAT_EQ(note.index, static_cast<float>(i));
    EXPECT_DOUBLE_EQ(note.offset_beats, kNoteOffsetBeats);

    // Remove note.
    score.Clear(i);
    EXPECT_TRUE(score.Empty(i));
  }
  EXPECT_TRUE(score.Empty());
}

// Tests that the score returns added notes as expected when they have the same
// offset.
TEST(ScoreTest, MultipleNotesSameOffsetBeats) {
  const int kNumNotes = 4;
  const double kNoteOffsetBeats = 0.5;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add |kNumNotes| notes using the same |kNoteOffsetBeats|.
  for (int i = 0; i < kNumNotes; ++i) {
    score.AddNote(kBeat, {static_cast<float>(i), kNoteIntensity,
                          kNoteOffsetBeats, kNoteDurationBeats});
  }
  EXPECT_FALSE(score.Empty());
  EXPECT_FALSE(score.Empty(kBeat));

  // Get all notes.
  const auto* notes = score.GetNotes(kBeat);
  ASSERT_NE(notes, nullptr);
  ASSERT_EQ(notes->size(), kNumNotes);

  for (int i = 0; i < kNumNotes; ++i) {
    const Note& note = (*notes)[i];
    EXPECT_FLOAT_EQ(note.index, static_cast<float>(i));
    EXPECT_DOUBLE_EQ(note.offset_beats, kNoteOffsetBeats);
  }

  score.Clear(kBeat);
  EXPECT_TRUE(score.Empty());
  EXPECT_TRUE(score.Empty(kBeat));
}

// Tests that clearing the score removes all existing notes as expected.
TEST(ScoreTest, Clear) {
  const int kNumBeats = 2;
  const int kNumNotes = 4;

  Score score;
  EXPECT_TRUE(score.Empty());

  for (int beat = 0; beat < kNumBeats; ++beat) {
    EXPECT_TRUE(score.Empty(beat));
    EXPECT_EQ(score.GetNotes(beat), nullptr);
    for (int i = 0; i < kNumNotes; ++i) {
      const double offset_beats =
          static_cast<double>(i) / static_cast<double>(kNumNotes);
      score.AddNote(
          beat, {kNoteIndex, kNoteIntensity, offset_beats, kNoteDurationBeats});
    }
    EXPECT_FALSE(score.Empty(beat));
    EXPECT_NE(score.GetNotes(beat), nullptr);
  }
  EXPECT_FALSE(score.Empty());

  score.Clear();

  for (int beat = 0; beat < kNumBeats; ++beat) {
    EXPECT_TRUE(score.Empty(beat));
    EXPECT_EQ(score.GetNotes(beat), nullptr);
  }
  EXPECT_TRUE(score.Empty());
}

}  // namespace
}  // namespace barelyapi
