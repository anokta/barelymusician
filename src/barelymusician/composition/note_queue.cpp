#include "barelymusician/composition/note_queue.h"

#include <algorithm>

namespace barelyapi {

namespace {

// Compares the given two notes with respect to their timestamps.
//
// @param lhs First note.
// @param rhs Second note.
// @return True if the first note comes prior to the second note.
bool CompareNote(const Note& lhs, const Note& rhs) {
  return lhs.timestamp < rhs.timestamp;
}

// Compares the given |note| against the given |timestamp|.
//
// @param note Note.
// @param timestamp Timestamp.
// @return True if the note comes prior to the timestamp.
bool CompareTimestamp(const Note& note, int timestamp) {
  return note.timestamp < timestamp;
}

}  // namespace

std::vector<Note> NoteQueue::Pop(int start_sample, int num_samples) {
  const auto begin = std::lower_bound(notes_.begin(), notes_.end(),
                                      start_sample, &CompareTimestamp);
  if (begin == notes_.end()) {
    // No notes found within the range, return an empty list.
    return std::vector<Note>();
  }

  const int end_sample = start_sample + num_samples;
  const auto end =
      std::lower_bound(begin, notes_.end(), end_sample, &CompareTimestamp);

  std::vector<Note> notes_in_range(begin, end);
  notes_.erase(begin, end);
  return notes_in_range;
}

void NoteQueue::Push(const Note& note) {
  const auto it =
      std::upper_bound(notes_.begin(), notes_.end(), note, &CompareNote);
  notes_.insert(it, note);
}

void NoteQueue::Reset() { notes_.clear(); }

}  // namespace barelyapi
