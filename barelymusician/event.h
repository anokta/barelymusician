#ifndef BARELYMUSICIAN_EVENT_H_
#define BARELYMUSICIAN_EVENT_H_

#include <variant>

#include "barelymusician/barelymusician.h"

namespace barelyapi {

/// Set data event.
struct SetDataEvent {
  explicit SetDataEvent(BarelyDataDefinition definition) noexcept
      : definition(definition) {}
  SetDataEvent() = default;
  ~SetDataEvent() {
    if (definition.data && definition.destroy_callback) {
      definition.destroy_callback(definition.data);
    }
  }
  SetDataEvent(const SetDataEvent& other) = delete;
  SetDataEvent& operator=(const SetDataEvent& other) = delete;
  SetDataEvent(SetDataEvent&& other) noexcept
      : definition(std::exchange(other.definition, {})) {
    other.definition.data = nullptr;
  }
  SetDataEvent& operator=(SetDataEvent&& other) noexcept {
    if (definition.data && definition.destroy_callback) {
      definition.destroy_callback(definition.data);
    }
    definition = std::exchange(other.definition, {});
    other.definition.data = nullptr;
    return *this;
  }

  /// Data definition.
  BarelyDataDefinition definition;
};

/// Set parameter event.
struct SetParameterEvent {
  /// Parameter index.
  int index;

  /// Parameter value.
  double value;
};

/// Start note event.
struct StartNoteEvent {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;
};

/// Stop note event.
struct StopNoteEvent {
  /// Note pitch.
  float pitch;
};

/// Instrument event type.
using Event = std::variant<SetDataEvent, SetParameterEvent, StartNoteEvent,
                           StopNoteEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_EVENT_H_
