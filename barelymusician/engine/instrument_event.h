#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>
#include <variant>

namespace barelyapi {

/// Custom data event.
struct CustomData {
  /// Custom data.
  std::any data;

  /// Default comparator.
  bool operator==(const CustomData&) const = default;
};

/// Note off event.
struct NoteOff {
  /// Note pitch.
  float pitch;

  /// Default comparator.
  bool operator==(const NoteOff&) const = default;
};

/// Note on event.
struct NoteOn {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;

  /// Default comparator.
  bool operator==(const NoteOn&) const = default;
};

/// Parameter event.
struct Param {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const Param&) const = default;
};

/// Instrument event type.
using InstrumentEvent = std::variant<CustomData, NoteOff, NoteOn, Param>;

/// Instrument event visitor.
template <class... EventTypes>
struct InstrumentEventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
InstrumentEventVisitor(EventTypes...) -> InstrumentEventVisitor<EventTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
