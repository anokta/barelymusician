#include "barelymusician/engine/note.h"

namespace barelyapi {

Note::Note(Definition definition) noexcept : definition_(definition) {}

const Note::Definition& Note::GetDefinition() const noexcept {
  return definition_;
}

}  // namespace barelyapi
