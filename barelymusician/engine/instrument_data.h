#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_DATA_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_DATA_H_

#include <variant>

namespace barelyapi {

/// Custom data.
struct CustomData {
  /// Custom data.
  void* data;

  /// Default comparator.
  bool operator==(const CustomData&) const = default;
};

/// Note off data.
struct NoteOff {
  /// Note pitch.
  float pitch;

  /// Default comparator.
  bool operator==(const NoteOff&) const = default;
};

/// Note on data.
struct NoteOn {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;

  /// Default comparator.
  bool operator==(const NoteOn&) const = default;
};

/// Parameter data.
struct Param {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const Param&) const = default;
};

/// Instrument data type.
using InstrumentData = std::variant<CustomData, NoteOff, NoteOn, Param>;

/// Instrument data visitor.
template <class... DataTypes>
struct InstrumentDataVisitor : DataTypes... {
  using DataTypes::operator()...;
};
template <class... DataTypes>
InstrumentDataVisitor(DataTypes...) -> InstrumentDataVisitor<DataTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_DATA_H_
