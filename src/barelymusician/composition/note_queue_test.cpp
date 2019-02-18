#include "barelymusician/composition/note_queue.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
constexpr int kNumSamples = 16;

// Test note parameters.
constexpr bool kNoteIsOn = true;
constexpr float kNoteIndex = 64.0f;
constexpr float kNoteIntensity = 1.0f;

// Helper method that checks equality of two notes.
//
// @param lhs First note.
// @param rhs Second note.
// @return True if two notes are equal.
bool AreNotesEqual(const Note& lhs, const Note& rhs) {
  return lhs.is_on == rhs.is_on && lhs.index == rhs.index &&
         lhs.intensity == rhs.intensity && lhs.timestamp == rhs.timestamp;
}

// Tests that the note queue returns an added single note as expected.
TEST(NoteQueueTest, SinglePushPop) {
  const int kTimestamp = 10;
  const Note kNote = {kNoteIsOn, kNoteIndex, kNoteIntensity, kTimestamp};

  NoteQueue note_queue;
  EXPECT_TRUE(note_queue.Pop(0, kNumSamples).empty());

  // Push single note.
  note_queue.Push(kNote);
  EXPECT_TRUE(note_queue.Pop(0, kTimestamp).empty());

  const auto notes_in_range = note_queue.Pop(0, kTimestamp + kNumSamples);
  ASSERT_EQ(1, notes_in_range.size());
  EXPECT_TRUE(AreNotesEqual(kNote, notes_in_range.front()));

  // Queue should be empty after pop.
  EXPECT_TRUE(note_queue.Pop(0, kTimestamp + kNumSamples).empty());
}

// Tests that the note queue returns added notes as expected when asked one at a
// time.
TEST(NoteQueueTest, SingleNotePerNumSamples) {
  const int kNumNotes = 5;

  NoteQueue note_queue;
  EXPECT_TRUE(note_queue.Pop(0, kNumSamples).empty());

  // Push |kNumNotes| notes, each note to the beginning of each |kNumSamples|.
  for (int i = 0; i < kNumNotes; ++i) {
    note_queue.Push({kNoteIsOn, kNoteIndex, kNoteIntensity, i * kNumSamples});
  }
  // Pop one note at a time.
  for (int i = 0; i < kNumNotes; ++i) {
    const int expected_timestamp = i * kNumSamples;
    const auto notes_in_range = note_queue.Pop(expected_timestamp, kNumSamples);
    ASSERT_EQ(1, notes_in_range.size());
    EXPECT_EQ(expected_timestamp, notes_in_range.front().timestamp);

    // There should be nothing left within the range after pop.
    EXPECT_TRUE(note_queue.Pop(expected_timestamp, kNumSamples).empty());
  }
}

// Tests that the note queue returns added notes as expected when they have the
// same timestamps.
TEST(NoteQueueTest, MultipleNotesSameTimestamp) {
  const int kNumNotes = 4;
  const int kTimestamp = 8;

  NoteQueue note_queue;
  EXPECT_TRUE(note_queue.Pop(0, kNumSamples).empty());

  // Push |kNumNotes| notes using the same |kTimestamp|.
  for (int i = 0; i < kNumNotes; ++i) {
    const float note_index = kNoteIndex + static_cast<float>(i);
    note_queue.Push({kNoteIsOn, note_index, kNoteIntensity, kTimestamp});
  }
  EXPECT_TRUE(note_queue.Pop(0, kTimestamp).empty());

  const auto notes_in_range = note_queue.Pop(kTimestamp, kNumSamples);
  EXPECT_EQ(kNumNotes, notes_in_range.size());
  for (const auto& note : notes_in_range) {
    EXPECT_EQ(kTimestamp, note.timestamp);
  }

  // Queue should be empty after pop.
  EXPECT_TRUE(note_queue.Pop(kTimestamp, kNumSamples).empty());
}

// Tests that resetting the queue clears out the existing notes as expected.
TEST(NoteQueueTest, Reset) {
  NoteQueue note_queue;
  EXPECT_TRUE(note_queue.Pop(0, kNumSamples).empty());

  for (int i = 0; i < kNumSamples; ++i) {
    note_queue.Push({kNoteIsOn, kNoteIndex, kNoteIntensity, i});
  }

  note_queue.Reset();
  EXPECT_TRUE(note_queue.Pop(0, kNumSamples).empty());
}

}  // namespace
}  // namespace barelyapi
