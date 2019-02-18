#ifndef BARELYMUSICIAN_BASE_NOTE_H_
#define BARELYMUSICIAN_BASE_NOTE_H_

namespace barelyapi {

// Basic musical note structure.
// Mostly based on MIDI messages for consistency.
struct Note {
  // Denotes whether the note is on.
  bool is_on = false;

  // Note index.
  float index = 0.0f;

  // Note intensity.
  float intensity = 0.0f;

  // Note timestamp in samples.
  int timestamp = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_NOTE_H_
