#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include <span>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/note.h"

namespace barelyapi {

/// Conductor that controls musical adjustments.
class Conductor {
 public:
  /// Adjust note callback alias.
  using AdjustNoteCallback = barely::Musician::AdjustNoteCallback;

  // TODO(#98): Add `AdjustParameterAutomationDefinitionCallback` functionality.

  /// Returns note.
  ///
  /// @param definition Note pitch definition.
  /// @return Note pitch.
  [[nodiscard]] double GetNote(Note::PitchDefinition definition) const noexcept;

  /// Returns root note.
  ///
  /// @return Root note pitch.
  [[nodiscard]] double GetRootNote() const noexcept;

  /// Returns scale.
  ///
  /// @return List of scale note pitches.
  [[nodiscard]] std::span<const double> GetScale() const noexcept;

  /// Sets adjust note definition callback.
  ///
  /// @param callback Adjust note definition callback.
  void SetAdjustNoteCallback(AdjustNoteCallback callback) noexcept;

  /// Sets root note.
  ///
  /// @param root_pitch Root note pitch.
  void SetRootNote(double root_pitch) noexcept;

  /// Sets scale.
  ///
  /// @param scale_pitches List of scale note pitches.
  void SetScale(std::span<const double> scale_pitches) noexcept;

  /// Transforms note.
  ///
  /// @param definition Note definition.
  /// @param skip_adjustment True to skip note adjustment.
  /// @return Note.
  [[nodiscard]] Note TransformNote(Note::Definition definition,
                                   bool skip_adjustment) const noexcept;

 private:
  // Adjust note callback.
  AdjustNoteCallback adjust_note_callback_;

  // Root note pitch.
  double root_pitch_ = 0.0;

  // List of scale note pitches.
  std::vector<double> scale_pitches_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
