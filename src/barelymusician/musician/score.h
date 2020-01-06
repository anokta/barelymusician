#ifndef BARELYMUSICIAN_MUSICIAN_SCORE_H_
#define BARELYMUSICIAN_MUSICIAN_SCORE_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/musician/note.h"

namespace barelyapi {

// Musical score.
class Score {
 public:
  // Adds new note to the given beat.
  //
  // @param beat Beat.
  // @param note Note.
  void AddNote(int beat, const Note& note);

  // Clears all the notes.
  void Clear();

  // Clears all the notes in the given beat.
  void Clear(int beat);

  // Returns whether the score is empty or not.
  //
  // @return True if empty.
  bool Empty() const;

  // Returns whether the score has the given beat is empty or not.
  //
  // @return True if beat exists.
  bool Empty(int beat) const;

  // Returns the notes for the given beat.
  //
  // @param beat Beat.
  // @return Pointer to list of notes, nullptr if does not exist.
  const std::vector<Note>* GetNotes(int beat) const;

 private:
  // Score notes for each beat.
  std::unordered_map<int, std::vector<Note>> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_SCORE_H_
