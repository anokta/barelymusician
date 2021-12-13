#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>
#include <map>
#include <utility>
#include <variant>

#include "barelymusician/common/id.h"

namespace barely {

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

/// Instrument event type.
using InstrumentEvent =
    std::variant<SetAllNotesOffEvent, SetAllParamsToDefaultEvent,
                 SetCustomDataEvent, SetNoteOffEvent, SetNoteOnEvent,
                 SetParamEvent, SetParamToDefaultEvent>;

/// Instrument id-event pair.
using InstrumentIdEventPair = std::pair<Id, InstrumentEvent>;

/// Instrument id-event pair by position map type.
using InstrumentIdEventPairMap = std::multimap<double, InstrumentIdEventPair>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
