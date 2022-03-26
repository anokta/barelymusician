#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Class that wraps note.
class Note {
 public:
  /// Definition alias.
  using Definition = barely::NoteDefinition;

  /// Constructs new `Note`.
  ///
  /// @param definition Note definition.
  explicit Note(Definition definition) noexcept;

  /// Returns definition.
  ///
  /// @return Note definition.
  [[nodiscard]] const Definition& GetDefinition() const noexcept;

 private:
  // Definition.
  Definition definition_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_H_
