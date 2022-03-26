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
  /// Pitch.
  double pitch;
};

/// Set note on event.
struct SetNoteOnEvent {
  /// Pitch.
  double pitch;

  /// Intensity.
  double intensity;
};

/// Set parameter event.
struct SetParameterEvent {
  /// Index.
  int index;

  /// Value.
  double value;

  /// Slope in value per second.
  double slope;
};

/// Event alias.
using Event = std::variant<SetDataEvent, SetNoteOffEvent, SetNoteOnEvent,
                           SetParameterEvent>;

// Event visitor.
template <class... EventTypes>
struct EventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
EventVisitor(EventTypes...) -> EventVisitor<EventTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_EVENT_H_
