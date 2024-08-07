#ifndef BARELYMUSICIAN_INTERNAL_NOTE_H_
#define BARELYMUSICIAN_INTERNAL_NOTE_H_

#include "barelymusician/internal/control.h"

namespace barely::internal {

/// Class that wraps a note.
class Note {
 public:
  /// Constructs a new `Control`.
  ///
  /// @param note_id Note identifier.
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param control_map Control map.
  Note(int note_id, double pitch, double intensity, ControlMap control_map) noexcept;

  /// Returns a control value.
  ///
  /// @param control_id Control identifier.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] Control* GetControl(int control_id) noexcept;

  /// Returns identifier.
  ///
  /// @return Note identifier.
  [[nodiscard]] int GetId() const noexcept;

  /// Returns intensity.
  ///
  /// @return Intensity.
  [[nodiscard]] double GetIntensity() const noexcept;

  /// Returns pitch.
  ///
  /// @return Pitch.
  [[nodiscard]] double GetPitch() const noexcept;

 private:
  // Note identifier.
  int note_id_ = 0;

  // Pitch.
  double pitch_ = 0.0;

  // Intensity.
  double intensity_ = 1.0;

  // Control map.
  ControlMap control_map_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_NOTE_H_
