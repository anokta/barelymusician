#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_

#include <variant>

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

// Parameter data.
struct ParamData {
  // Param id.
  int id;

  // Param value.
  float value;
};

// Message data type.
using MessageData = std::variant<NoteOnData, NoteOffData, ParamData>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
