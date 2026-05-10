#ifndef BARELYMUSICIAN_ENGINE_EVENT_STATE_H_
#define BARELYMUSICIAN_ENGINE_EVENT_STATE_H_

#include <barelymusician.h>

#include <cstdint>
#include <variant>

namespace barely {

struct ControlEvent {
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;
  float value = 0.0f;
};

struct NoteControlEvent {
  BarelyNoteControlType type = BarelyNoteControlType_kCount;
  float pitch = 0.0f;
  float value = 0.0f;
};

enum class EventType : uint8_t {
  kControlEvent = 0,
  kNoteControlEvent,
  kNoteOffEvent,
  kNoteOnEvent,
};

struct EventState {
  union {
    ControlEvent control;
    NoteControlEvent note_control;
    float pitch = 0.0f;
  };

  double position = 0.0;

  uint32_t prev_event_index = kInvalidIndex;
  uint32_t next_event_index = kInvalidIndex;

  EventType type;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_EVENT_STATE_H_
