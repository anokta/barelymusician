#ifndef BARELYMUSICIAN_MUSICIAN_NOTE_H_
#define BARELYMUSICIAN_MUSICIAN_NOTE_H_

namespace barelyapi {

// Musical note.
struct Note {
  // Note index (typically key).
  float index;

  // Note intensity (typically loudness).
  float intensity;

  // Note offset in beats from the start beat.
  float offset_beats;

  // Note duration in beats.
  float duration_beats;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_NOTE_H_
