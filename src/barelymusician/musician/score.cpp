#include "barelymusician/musician/score.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/musician/note_utils.h"

namespace barelyapi {

void Score::AddNote(int beat, const Note& note) {
  const auto [it, success] =
      notes_.insert(std::pair(beat, std::vector<Note>(1, note)));
  if (!success) {
    auto* notes = &it->second;
    const auto note_it =
        std::upper_bound(notes->cbegin(), notes->cend(), note, &CompareNote);
    notes->insert(note_it, note);
  }
}

void Score::Clear() { notes_.clear(); }

void Score::Clear(int beat) { notes_.erase(beat); }

bool Score::Empty() const { return notes_.empty(); }

bool Score::Empty(int beat) const { return notes_.find(beat) == notes_.cend(); }

const std::vector<Note>* Score::GetNotes(int beat) const {
  const auto it = notes_.find(beat);
  if (it == notes_.cend()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace barelyapi
