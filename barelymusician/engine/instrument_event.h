#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>
#include <map>
#include <variant>

namespace barelyapi {

/// Reset all parameters event.
struct ResetAllParams {};

/// Reset parameter event.
struct ResetParam {
  /// Parameter id.
  int id;
};

/// Set all notes off event.
struct SetAllNotesOff {};

/// Set custom data event.
struct SetCustomData {
  /// Custom data.
  std::any data;
};

/// Set note off event.
struct SetNoteOff {
  /// Note pitch.
  float pitch;
};

/// Set note on event.
struct SetNoteOn {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;
};

/// Set parameter event.
struct SetParam {
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

/// Instrument controller event type.
using InstrumentControllerEvent =
    std::variant<ResetAllParams, ResetParam, SetAllNotesOff, SetCustomData,
                 SetNoteOff, SetNoteOn, SetParam>;

// Instrument controller events with their timestamps container type.
using InstrumentControllerEvents =
    std::multimap<double, InstrumentControllerEvent>;

/// Instrument processor event type.
using InstrumentProcessorEvent =
    std::variant<SetCustomData, SetNoteOff, SetNoteOn, SetParam>;

// Instrument processor events with their timestamps container type.
using InstrumentProcessorEvents =
    std::multimap<double, InstrumentProcessorEvent>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
