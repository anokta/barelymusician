#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_

#include <variant>

namespace barelyapi {

// Custom data.
struct CustomData {
  // Custom data.
  void* data;
};

// Note off data.
struct NoteOff {
  // Note pitch.
  float pitch;
};

// Note on data.
struct NoteOn {
  // Note pitch.
  float pitch;
  // Note intensity.
  float intensity;
};

// Param data.
struct Param {
  // Param id.
  int id;
  // Param value.
  float value;
};

// Instrument data type.
using InstrumentData = std::variant<CustomData, NoteOff, NoteOn, Param>;

// Instrument data visitor.
template <class... DataTypes>
struct InstrumentDataVisitor : DataTypes... {
  using DataTypes::operator()...;
};
template <class... DataTypes>
InstrumentDataVisitor(DataTypes...) -> InstrumentDataVisitor<DataTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
