#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_DATA_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_DATA_H_

namespace barelyapi {

// Note on data.
struct NoteOnData {
  // Note index.
  float index;

  // Note intensity.
  float intensity;
};

// Note off data.
struct NoteOffData {
  // Note index.
  float index;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_DATA_H_
