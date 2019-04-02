#include "barelymusician/composition/bar.h"

#include "barelymusician/composition/note.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Test beat duration to retrieve notes.
const float kDurationBeats = 1.0f;

// Test note properties.
const float kNoteIndex = 12.0f;
const float kNoteIntensity = 1.0f;
const float kNoteDurationBeats = 0.5f;

// Tests that a single note added to the bar can be retrieved as expected.
TEST(BarTest, SingleNote) {
  const float kNoteStartBeat = 0.1f;

  Bar bar;
  EXPECT_TRUE(bar.GetNotes(kNoteStartBeat, kDurationBeats).empty());

  // Add single note.
  bar.AddNote({kNoteIndex, kNoteIntensity, kNoteStartBeat, kNoteDurationBeats});

  // Get note.
  const auto notes_in_range = bar.GetNotes(kNoteStartBeat, kDurationBeats);
  ASSERT_EQ(1, notes_in_range.size());

  const auto& note = notes_in_range.front();
  EXPECT_EQ(kNoteIndex, note.index);
  EXPECT_EQ(kNoteIntensity, note.intensity);
  EXPECT_EQ(kNoteStartBeat, note.start_beat);
  EXPECT_EQ(kNoteDurationBeats, note.duration_beats);
}

TEST(BarTest, MultipleNotes) {
  const int kNumNotes = 6;

  Bar bar;
  EXPECT_TRUE(bar.GetNotes(0.0f, kDurationBeats).empty());

  // Add |kNumNotes| notes, each note to the beginning of each |kDurationBeats|.
  for (int i = 0; i < kNumNotes; ++i) {
    const float start_beat = static_cast<float>(i * kDurationBeats);
    bar.AddNote({kNoteIndex, kNoteIntensity, start_beat, kNoteDurationBeats});
  }

  // Get one note at a time.
  for (int i = 0; i < kNumNotes; ++i) {
    const float expected_start_beat = static_cast<float>(i * kDurationBeats);
    const auto notes_in_range =
        bar.GetNotes(expected_start_beat, kDurationBeats);
    ASSERT_EQ(1, notes_in_range.size());

    const auto& note = notes_in_range.front();
    EXPECT_EQ(kNoteIndex, note.index);
    EXPECT_EQ(kNoteIntensity, note.intensity);
    EXPECT_EQ(expected_start_beat, note.start_beat);
    EXPECT_EQ(kNoteDurationBeats, note.duration_beats);
  }
}

// Tests that resetting the bar clears out all the existing notes as expected.
TEST(BarTest, Reset) {
  const int kNumNotes = 8;

  Bar bar;
  EXPECT_TRUE(bar.GetNotes(0.0f, kDurationBeats).empty());

  // Add |kNumNotes| notes, equally spaced in |kDurationBeats|.
  const float beat_increment = kDurationBeats / static_cast<float>(kNumNotes);
  for (int i = 0; i < kNumNotes; ++i) {
    const float start_beat = static_cast<float>(i) * beat_increment;
    bar.AddNote({kNoteIndex, kNoteIntensity, start_beat, kNoteDurationBeats});
  }
  EXPECT_EQ(kNumNotes, bar.GetNotes(0.0f, kDurationBeats).size());

  // Reset the bar.
  bar.Reset();
  EXPECT_TRUE(bar.GetNotes(0.0f, kDurationBeats).empty());
}

}  // namespace
}  // namespace barelyapi
