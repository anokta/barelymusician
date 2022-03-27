#ifndef BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
#define BARELYMUSICIAN_ENGINE_CONDUCTOR_H_

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/note.h"

namespace barelyapi {

/// Conductor that controls musical adjustments.
class Conductor {
 public:
  /// Adjust note definition callback alias.
  using AdjustNoteDefinitionCallback =
      barely::Musician::AdjustNoteDefinitionCallback;

  /// Adjust tempo callback alias.
  using AdjustTempoCallback = barely::Musician::AdjustTempoCallback;

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
  [[nodiscard]] const std::vector<double>& GetScale() const noexcept;

  /// Sets adjust note definition callback.
  ///
  /// @param callback Adjust note definition callback.
  void SetAdjustNoteDefinitionCallback(
      AdjustNoteDefinitionCallback callback) noexcept;

  /// Sets adjust tempo callback.
  ///
  /// @param callback Adjust tempo callback.
  void SetAdjustTempoCallback(AdjustTempoCallback callback) noexcept;

  /// Sets root note.
  ///
  /// @param root_pitch Root note pitch.
  void SetRootNote(double root_pitch) noexcept;

  /// Sets scale.
  ///
  /// @param scale_pitches List of scale note pitches.
  void SetScale(std::vector<double> scale_pitches) noexcept;

  /// Transforms note.
  ///
  /// @param definition Note definition.
  /// @param bypass_adjustment True to bypass note adjustment.
  /// @return Note.
  [[nodiscard]] Note TransformNote(Note::Definition definition,
                                   bool bypass_adjustment) noexcept;

  /// Transforms tempo.
  ///
  /// @param tempo Tempo in bpm.
  /// @return Tempo in bpm.
  [[nodiscard]] double TransformTempo(double tempo) noexcept;

 private:
  // Adjust note definition callback.
  AdjustNoteDefinitionCallback adjust_note_definition_callback_;

  // Adjust tempo callback.
  AdjustTempoCallback adjust_tempo_callback_;

  // Root note pitch.
  double root_pitch_ = 0.0;

  // List of scale note pitches.
  std::vector<double> scale_pitches_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_CONDUCTOR_H_
