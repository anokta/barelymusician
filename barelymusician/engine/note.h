#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

#include <compare>

namespace barelyapi {

/// Class that wraps note.
struct Note {
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
