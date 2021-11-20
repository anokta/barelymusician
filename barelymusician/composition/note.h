#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_H_

namespace barelyapi {

/// Musical note.
struct Note {
  /// Note pitch.
  // TODO: Use |NotePitch| instead.
  float pitch;

  /// Note intensity.
  // TODO: Use |NoteIntensity| instead.
  float intensity;

  /// Note duration.
  // TODO: Use |NoteDuration| instead.
  double duration;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_H_
