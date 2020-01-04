#ifndef BARELYMUSICIAN_MUSICIAN_SCORE_H_
#define BARELYMUSICIAN_MUSICIAN_SCORE_H_

#include <vector>

#include "barelymusician/musician/note.h"

namespace barelyapi {

// Musical score.
class Score {
 public:
  // Score iterator.
  struct Iterator {
    // Iterator begin.
    std::vector<Note>::const_iterator begin;

    // Iterator end.
    std::vector<Note>::const_iterator end;
  };

  // Adds new note.
  //
  // @param note Note.
  void AddNote(const Note& note);

  // Clears all the notes.
  void Clear();

  // Clears the notes within the given range.
  //
  // @param iterator Iterator range to clear the notes.
  void Clear(const Iterator& iterator);

  // Returns whether the score is empty or not.
  //
  // @return True if empty.
  bool Empty() const;

  // Returns iterator within the given range.
  //
  // @param start_position Start position in beats.
  // @param end_position End position in beats.
  Iterator GetIterator(float start_position, float end_position) const;

 private:
  // Score notes.
  std::vector<Note> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_SCORE_H_
