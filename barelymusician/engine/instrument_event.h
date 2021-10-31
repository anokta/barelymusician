#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>
#include <map>
#include <utility>
#include <variant>

#include "barelymusician/common/id.h"
#include "barelymusician/engine/instrument_definition.h"

namespace barelyapi {

/// Create event.
struct CreateEvent {
  /// Instrument definition.
  InstrumentDefinition definition;
};

/// Destroy event.
struct DestroyEvent {};

/// Set all notes off event.
struct SetAllNotesOffEvent {};

/// Set all parameters to default value event.
struct SetAllParamsToDefaultEvent {};

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

/// Set parameter value event.
struct SetParamEvent {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;
};

/// Set parameter to default value event.
struct SetParamToDefaultEvent {
  /// Parameter id.
  int id;
};

/// Instrument controller event type.
using InstrumentControllerEvent =
    std::variant<SetAllNotesOffEvent, SetAllParamsToDefaultEvent,
                 SetCustomDataEvent, SetNoteOffEvent, SetNoteOnEvent,
                 SetParamEvent, SetParamToDefaultEvent>;

/// Instrument controller id-event pairs by their timestamps container type.
using InstrumentControllerEvents =
    std::multimap<double, std::pair<Id, InstrumentControllerEvent>>;

/// Instrument processor event type.
using InstrumentProcessorEvent =
    std::variant<CreateEvent, DestroyEvent, SetCustomDataEvent, SetNoteOffEvent,
                 SetNoteOnEvent, SetParamEvent>;

/// Instrument events by their timestamps container type.
using InstrumentProcessorEvents =
    std::multimap<double, InstrumentProcessorEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
