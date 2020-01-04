#include "barelymusician/musician/score.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

// Compares the given two messages with respect to their start positions.
//
// @param lhs First note.
// @param rhs Second note.
// @return True if the first note comes prior to the second note.
bool CompareNote(const Note& lhs, const Note& rhs) {
  return (lhs.start_beat == rhs.start_beat)
             ? lhs.offset_beats < rhs.offset_beats
             : lhs.start_beat < rhs.start_beat;
}

// Compares the given |note| against the given |position|.
//
// @param note Note.
// @param position Position in beats.
// @return True if the note comes prior to the position.
bool ComparePosition(const Note& note, float position) {
  return static_cast<float>(note.start_beat) + note.offset_beats < position;
}

}  // namespace

void Score::AddNote(const Note& note) {
  const auto it =
      std::upper_bound(notes_.begin(), notes_.end(), note, &CompareNote);
  notes_.insert(it, note);
}

void Score::Clear() { notes_.clear(); }

void Score::Clear(const Iterator& iterator) {
  notes_.erase(iterator.begin, iterator.end);
}

bool Score::Empty() const { return notes_.empty(); }

Score::Iterator Score::GetIterator(float start_position,
                                   float end_position) const {
  DCHECK_GE(start_position, 0.0f);
  DCHECK_LE(start_position, end_position);
  Iterator iterator;
  iterator.begin = std::lower_bound(notes_.begin(), notes_.end(),
                                    start_position, &ComparePosition);
  iterator.end = std::lower_bound(iterator.begin, notes_.end(), end_position,
                                  &ComparePosition);
  return iterator;
}

}  // namespace barelyapi
