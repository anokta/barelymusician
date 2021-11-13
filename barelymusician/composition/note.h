#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_H_

namespace barelyapi {

/// Musical note.
struct Note {
  /// Note begin position in beats.
  double begin_position;

  /// Note end position in beats.
  double end_position;

  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;

// TODO: temp to keep changes easy.
  double duration = 0.0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_H_
