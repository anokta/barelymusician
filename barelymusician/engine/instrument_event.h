#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_

#include <any>

namespace barelyapi {

/// Reset all parameters event.
struct ResetAllParams {
  /// Default comparator.
  bool operator==(const ResetAllParams&) const = default;
};

/// Reset parameter event.
struct ResetParam {
  /// Parameter id.
  int id;

  /// Default comparator.
  bool operator==(const ResetParam&) const = default;
};

/// Set all notes off event.
struct SetAllNotesOff {
  /// Default comparator.
  bool operator==(const SetAllNotesOff&) const = default;
};

/// Set custom data event.
struct SetCustomData {
  /// Custom data.
  std::any data;

  /// Default comparator.
  bool operator==(const SetCustomData&) const = default;
};

/// Set note off event.
struct SetNoteOff {
  /// Note pitch.
  float pitch;

  /// Default comparator.
  bool operator==(const SetNoteOff&) const = default;
};

/// Set note on event.
struct SetNoteOn {
  /// Note pitch.
  float pitch;

  /// Note intensity.
  float intensity;

  /// Default comparator.
  bool operator==(const SetNoteOn&) const = default;
};

/// Set parameter event.
struct SetParam {
  /// Parameter id.
  int id;

  /// Parameter value.
  float value;

  /// Default comparator.
  bool operator==(const SetParam&) const = default;
};

/// Instrument event visitor.
template <class... EventTypes>
struct InstrumentEventVisitor : EventTypes... {
  using EventTypes::operator()...;
};
template <class... EventTypes>
InstrumentEventVisitor(EventTypes...) -> InstrumentEventVisitor<EventTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_EVENT_H_
