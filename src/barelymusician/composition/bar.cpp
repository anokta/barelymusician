#include "barelymusician/composition/bar.h"

#include <algorithm>

namespace barelyapi {

namespace {

// Compares the given two notes with respect to their start beats.
//
// @param lhs First note.
// @param rhs Second note.
// @return True if the first note comes prior to the second note.
bool CompareNote(const Note& lhs, const Note& rhs) {
  return lhs.start_beat < rhs.start_beat;
}

// Compares the given |note| against the given |start_beat|.
//
// @param note Note.
// @param start_beat Start beat.
// @return True if the note comes prior to the start beat.
bool CompareStartBeat(const Note& note, float start_beat) {
  return note.start_beat < start_beat;
}

}  // namespace

void Bar::AddNote(const Note& note) {
  const auto it =
      std::upper_bound(notes_.begin(), notes_.end(), note, &CompareNote);
  notes_.insert(it, note);
}

std::vector<Note> Bar::GetNotes(float start_beat, float duration_beats) const {
  const auto begin = std::lower_bound(notes_.begin(), notes_.end(), start_beat,
                                      &CompareStartBeat);
  if (begin == notes_.end()) {
    // No notes found within the range, return an empty list.
    return std::vector<Note>();
  }

  const float end_beat = start_beat + duration_beats;
  const auto end =
      std::lower_bound(begin, notes_.end(), end_beat, &CompareStartBeat);

  return std::vector<Note>(begin, end);
}

void Bar::Reset() { notes_.clear(); }

}  // namespace barelyapi
