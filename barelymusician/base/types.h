#ifndef BARELYMUSICIAN_BASE_TYPES_H_
#define BARELYMUSICIAN_BASE_TYPES_H_

#include <cstdint>
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

// Variant visitor.
template <class... DataTypes>
struct Visitor : DataTypes... {
  using DataTypes::operator()...;
};
template <class... DataTypes>
Visitor(DataTypes...) -> Visitor<DataTypes...>;

// Id type.
using Id = std::int64_t;

// Message data type.
using MessageData = std::variant<NoteOnData, NoteOffData>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_TYPES_H_
