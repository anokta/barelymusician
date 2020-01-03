#ifndef BARELYMUSICIAN_MUSICIAN_NOTE_H_
#define BARELYMUSICIAN_MUSICIAN_NOTE_H_

namespace barelyapi {

// Musical note.
struct Note {
  // Note index (typically key).
  float index;

  // Note intensity (typically loudness).
  float intensity;

  // Note start beat.
  float start_beat;

  // Note duration in beats.
  float duration_beats;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MUSICIAN_NOTE_H_
