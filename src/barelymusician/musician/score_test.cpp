#include "barelymusician/musician/score.h"

#include <iterator>

#include "barelymusician/musician/note.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

const float kNoteIntensity = 1.0f;
const float kNoteDurationBeats = 0.5f;

// Tests that the score returns an added single note as expected.
TEST(ScoreTest, SingleNote) {
  const float kNoteIndex = 60.0f;
  const int kNoteStartBeat = 3;
  const float kNoteOffsetBeats = 0.25f;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add note.
  score.AddNote({kNoteIndex, kNoteIntensity, kNoteStartBeat, kNoteOffsetBeats,
                 kNoteDurationBeats});
  EXPECT_FALSE(score.Empty());

  // Get note.
  const float start_position =
      static_cast<float>(kNoteStartBeat) + kNoteOffsetBeats;
  const float end_position = start_position + kNoteDurationBeats;
  const auto iterator = score.GetIterator(start_position, end_position);
  ASSERT_EQ(std::distance(iterator.begin, iterator.end), 1);

  const Note& note = *iterator.begin;
  EXPECT_FLOAT_EQ(note.index, kNoteIndex);
  EXPECT_FLOAT_EQ(note.intensity, kNoteIntensity);
  EXPECT_EQ(note.start_beat, kNoteStartBeat);
  EXPECT_FLOAT_EQ(note.offset_beats, kNoteOffsetBeats);
  EXPECT_FLOAT_EQ(note.duration_beats, kNoteDurationBeats);

  // Remove note.
  score.Clear(iterator);
  EXPECT_TRUE(score.Empty());
}

// Tests that the score returns added notes as expected.
TEST(ScoreTest, MultipleNotes) {
  const int kNumNotes = 5;
  const float kNoteOffsetBeats = 0.75f;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add |kNumNotes| notes, each note with |kNoteOffsetBeats| from each beat.
  for (int i = 0; i < kNumNotes; ++i) {
    score.AddNote({static_cast<float>(i), kNoteIntensity, i, kNoteOffsetBeats,
                   kNoteDurationBeats});
    EXPECT_FALSE(score.Empty());
  }

  // Get one note at a time.
  for (int i = 0; i < kNumNotes; ++i) {
    const float start_position = static_cast<float>(i);
    const float end_position = static_cast<float>(i + 1);
    const auto iterator = score.GetIterator(start_position, end_position);
    ASSERT_EQ(std::distance(iterator.begin, iterator.end), 1);

    const Note& note = *iterator.begin;
    EXPECT_FLOAT_EQ(note.index, static_cast<float>(i));
    EXPECT_EQ(note.start_beat, i);
    EXPECT_EQ(note.offset_beats, kNoteOffsetBeats);

    // Remove note.
    score.Clear(iterator);
  }
  EXPECT_TRUE(score.Empty());
}

// Tests that the score returns added notes as expected when they have the same
// position.
TEST(ScoreTest, MultipleNotesSamePosition) {
  const int kNumNotes = 4;
  const int kStartBeat = 6;
  const float kNoteOffsetBeats = 0.5f;

  Score score;
  EXPECT_TRUE(score.Empty());

  // Add |kNumNotes| notes using the same |kStartBeat| and |kNoteOffsetBeats|.
  for (int i = 0; i < kNumNotes; ++i) {
    score.AddNote({static_cast<float>(i), kNoteIntensity, kStartBeat,
                   kNoteOffsetBeats, kNoteDurationBeats});
    EXPECT_FALSE(score.Empty());
  }

  // Get all notes.
  const float start_position =
      static_cast<float>(kStartBeat) + kNoteOffsetBeats;
  const float end_position = start_position + kNoteDurationBeats;
  const auto iterator = score.GetIterator(start_position, end_position);
  ASSERT_EQ(std::distance(iterator.begin, iterator.end), kNumNotes);

  auto it = iterator.begin;
  for (int i = 0; i < kNumNotes; ++i) {
    const Note& note = *it++;
    EXPECT_FLOAT_EQ(note.index, static_cast<float>(i));
    EXPECT_EQ(note.start_beat, kStartBeat);
    EXPECT_EQ(note.offset_beats, kNoteOffsetBeats);
  }

  score.Clear(iterator);
  EXPECT_TRUE(score.Empty());
}

// Tests that clearing the score removes all existing notes as expected.
TEST(ScoreTest, Clear) {
  const int kNumNotes = 10;
  const int kStartBeat = 5;

  Score score;
  EXPECT_TRUE(score.Empty());

  const float start_position = static_cast<float>(kStartBeat);
  const float end_position = static_cast<float>(kStartBeat + 1);
  Score::Iterator iterator;
  iterator = score.GetIterator(start_position, end_position);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), 0);

  for (int i = 0; i < kNumNotes; ++i) {
    const float offset_beats =
        static_cast<float>(i) / static_cast<float>(kNumNotes);
    score.AddNote(
        {0.0f, kNoteIntensity, kStartBeat, offset_beats, kNoteDurationBeats});
    EXPECT_FALSE(score.Empty());
  }
  iterator = score.GetIterator(start_position, end_position);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), kNumNotes);

  score.Clear();
  EXPECT_TRUE(score.Empty());

  iterator = score.GetIterator(start_position, end_position);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), 0);
}

}  // namespace
}  // namespace barelyapi
