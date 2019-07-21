#ifndef BARELYMUSICIAN_COMPOSITION_SCORE_H_
#define BARELYMUSICIAN_COMPOSITION_SCORE_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/composition/note.h"

namespace barelyapi {

// Musical score.
// TODO(#55): Refactor to add const accessors to existing notes.
class Score {
 public:
  // Clears all notes.
  void Clear();

  // Returns mutable score notes.
  //
  // @param section_type Section type.
  // @param bar Bar index.
  // @param beat Beat index.
  std::vector<Note>* GetNotes(int section_type, int bar, int beat);

 private:
  // Score notes.
  std::unordered_map<int, std::vector<std::vector<std::vector<Note>>>> notes_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_SCORE_H_
