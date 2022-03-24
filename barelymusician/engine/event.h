#ifndef BARELYMUSICIAN_ENGINE_EVENT_H_
#define BARELYMUSICIAN_ENGINE_EVENT_H_

#include <variant>

#include "barelymusician/engine/data.h"

namespace barelyapi {

/// Set data event.
struct SetDataEvent {
  /// Data.
  Data data;
};

/// Set note off event.
struct SetNoteOffEvent {
  /// Note pitch.
  double pitch;
};

/// Set note on event.
struct SetNoteOnEvent {
  /// Note pitch.
  double pitch;

  /// Note intensity.
  double intensity;
};

/// Set parameter event.
struct SetParameterEvent {
  /// Parameter index.
  int index;

  /// Parameter value.
  double value;

  /// Parameter slope in value per second.
  double slope;
};

/// Instrument event alias.
using Event = std::variant<SetDataEvent, SetNoteOffEvent, SetNoteOnEvent,
                           SetParameterEvent>;

// Instrument event visitor.
template <class... EventTypes>
struct EventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
EventVisitor(EventTypes...) -> EventVisitor<EventTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_EVENT_H_
