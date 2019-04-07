#ifndef BARELYMUSICIAN_COMPOSITION_BAR_H_
#define BARELYMUSICIAN_COMPOSITION_BAR_H_

#include <list>
#include <vector>

#include "barelymusician/composition/note.h"

namespace barelyapi {

// Musical bar (phrase) that consists of a list of notes.
class Bar {
public:
  // Adds a new note into the bar.
  //
  // @param note Note.
  void AddNote(const Note& note);

  // Returns all notes within the range from the bar.
  //
  // @param start_beat Start beat to begin.
  // @param duration_beats Duration beats to end.
  // @param List of notes within the range.
  std::vector<Note> GetNotes(float start_beat, float duration_beats) const;

  // Resets the bar.
  void Reset();

private:
  // Ordered note list.
  std::list<Note> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_BAR_H_
