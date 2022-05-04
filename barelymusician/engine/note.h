#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

#include <compare>

namespace barely::internal {

/// Class that wraps note.
struct Note {
  /// Default comparators.
  auto operator<=>(const Note& other) const noexcept = default;

  /// Duration.
  double duration = 0.0;

  /// Pitch.
  double pitch = 0.0;

  /// Intensity.
  double intensity = 1.0;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_NOTE_H_
