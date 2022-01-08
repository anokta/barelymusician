#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_EVENT_H_

#include <map>
#include <variant>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/instrument_event.h"

namespace barelyapi {

/// Create event.
struct CreateEvent {
  /// Instrument definition.
  InstrumentDefinition definition;
};

/// Destroy event.
struct DestroyEvent {};

/// Instrument processor event type.
using InstrumentProcessorEvent =
    std::variant<CreateEvent, DestroyEvent, SetDataEvent, SetGainEvent,
                 SetNoteOffEvent, SetNoteOnEvent, SetParamEvent>;

/// Instrument processor event by timestamp map type.
using InstrumentProcessorEventMap =
    std::multimap<double, InstrumentProcessorEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_EVENT_H_
