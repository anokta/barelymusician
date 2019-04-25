#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_H_

namespace barelyapi {

// Musical note.
struct Note {
  // Note index (typically key).
  float index;

  // Note intensity (typically loudness).
  float intensity;

  // Start beat of the note.
  float start_beat;

  // Duration of the note in beats.
  float duration_beats;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_H_
