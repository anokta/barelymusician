#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_

#include <unordered_set>
#include <variant>

namespace barelyapi {

/// Chord that wraps a set of pitches.
struct Chord {
  /// Chord pitches.
  std::unordered_set<NotePitch> pitches;
};

/// Note pitch type.
using NotePitch = std::variant<float, Chord>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_
