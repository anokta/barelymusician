#ifndef BARELYMUSICIAN_ENGINE_NOTE_H_
#define BARELYMUSICIAN_ENGINE_NOTE_H_

namespace barelyapi {

struct QuantizedNoteIndex {
  // Root note index.
  float root_index;

  // Scale index.
  int scale_index;
};

// Abstract musical note.
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
