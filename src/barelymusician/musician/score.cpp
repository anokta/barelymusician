#include "barelymusician/musician/score.h"

#include "barelymusician/base/logging.h"

namespace barelyapi {

void Score::Clear() { notes_.clear(); }

std::vector<Note>* Score::GetNotes(int section_type, int bar, int beat) {
  DCHECK_GE(bar, 0);
  DCHECK_GE(beat, 0);

  auto& section_notes = notes_[section_type];
  if (section_notes.size() <= bar) {
    section_notes.resize(bar + 1);
  }
  auto& bar_notes = section_notes[bar];
  if (bar_notes.size() <= beat) {
    bar_notes.resize(beat + 1);
  }
  return &bar_notes[beat];
}

}  // namespace barelyapi
