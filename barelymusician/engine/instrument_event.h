#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>
#include <map>
#include <variant>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Create event.
struct CreateEvent {
  /// Instrument definition.
  InstrumentDefinition definition;
};

/// Destroy event.
struct DestroyEvent {};

/// Set custom data event.
struct SetCustomDataEvent {
  /// Custom data.
  std::any data;
};

/// Set note off event.
struct SetNoteOffEvent {
  /// Note pitch.
  float pitch;
};

/// Set note on event.
struct SetNoteOnEvent {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;
};

/// Set parameter event.
struct SetParamEvent {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;
};

/// Instrument event visitor.
template <class... EventTypes>
struct InstrumentEventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
InstrumentEventVisitor(EventTypes...) -> InstrumentEventVisitor<EventTypes...>;

/// Instrument event type.
using InstrumentEvent =
    std::variant<CreateEvent, DestroyEvent, SetCustomDataEvent, SetNoteOffEvent,
                 SetNoteOnEvent, SetParamEvent>;

// Instrument events with their timestamps container type.
using InstrumentEvents = std::multimap<double, InstrumentEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
