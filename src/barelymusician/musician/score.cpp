#include "barelymusician/musician/score.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/musician/note_utils.h"

namespace barelyapi {

void Score::AddNote(int beat, const Note& note) {
  const auto result =
      notes_.insert(std::make_pair(beat, std::vector<Note>(1, note)));
  if (!result.second) {
    auto* notes = &result.first->second;
    const auto it =
        std::upper_bound(notes->begin(), notes->end(), note, &CompareNote);
    notes->insert(it, note);
  }
}

void Score::Clear() { notes_.clear(); }

void Score::Clear(int beat) { notes_.erase(beat); }

bool Score::Empty() const { return notes_.empty(); }

bool Score::Empty(int beat) const { return notes_.find(beat) == notes_.end(); }

const std::vector<Note>* Score::GetNotes(int beat) const {
  const auto it = notes_.find(beat);
  if (it == notes_.end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace barelyapi
