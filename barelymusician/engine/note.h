#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

namespace barelyapi {

// Musical note.
struct Note {
  // Note start position in beats.
  double position;

  // Note duration in beats.
  double duration;

  // Note index (typically key).
  float index;

  // Note intensity (typically loudness).
  float intensity;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_NOTE_H_
