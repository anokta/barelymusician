#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <map>
#include <utility>
#include <variant>

#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Create event.
struct CreateEvent {
  /// Definition.
  InstrumentDefinition definition;

  /// Sampling rate in hz.
  int sample_rate;
};

/// Destroy event.
struct DestroyEvent {};

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
struct SetParamEvent {
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
using InstrumentEvent =
    std::variant<CreateEvent, DestroyEvent, SetDataEvent, SetGainEvent,
                 SetParamEvent, StartNoteEvent, StopNoteEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
