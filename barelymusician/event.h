#ifndef BARELYMUSICIAN_EVENT_H_
#define BARELYMUSICIAN_EVENT_H_

#include <variant>

namespace barelyapi {

/// Set data event.
struct SetDataEvent {
  /// Data.
  void* data;
};

/// Set gain event.
struct SetGainEvent {
  /// Gain.
  float gain;
};

/// Set parameter event.
struct SetParameterEvent {
  /// Parameter index.
  int index;

  /// Parameter value.
  float value;
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
using Event = std::variant<SetDataEvent, SetGainEvent, SetParameterEvent,
                           StartNoteEvent, StopNoteEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_EVENT_H_
