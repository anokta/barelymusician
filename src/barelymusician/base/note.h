#ifndef BARELYMUSICIAN_BASE_NOTE_H_
#define BARELYMUSICIAN_BASE_NOTE_H_

namespace barelyapi {

// Basic musical note structure.
struct Note {
  // Note index.
  float index = 0.0f;

  // Note intensity.
  float intensity = 0.0f;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_NOTE_H_
