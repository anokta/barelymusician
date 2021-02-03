#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_H_

namespace barelyapi {

/// Musical note.
struct Note {
  /// Note start position in beats.
  double position;

  /// Note duration in beats.
  double duration;

  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_H_
