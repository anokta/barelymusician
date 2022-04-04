#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

#include <compare>

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Class that wraps note.
struct Note {
 public:
  /// Definition alias.
  using Definition = barely::NoteDefinition;

  /// Pitch definition alias.
  using PitchDefinition = barely::NotePitchDefinition;

  /// Pitch type.
  using PitchType = barely::NotePitchType;

  /// Constructs new `Note`.
  ///
  /// @param duration Duration.
  /// @param pitch Pitch.
  /// @param intensity Intensity.
  Note(double duration, double pitch, double intensity)
      : duration(duration), pitch(pitch), intensity(intensity) {}

  /// Default comparators.
  auto operator<=>(const Note& other) const noexcept = default;

  /// Duration.
  double duration = 0.0;

  /// Pitch.
  double pitch = 0.0;

  /// Intensity.
  double intensity = 0.0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_H_
