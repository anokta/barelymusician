#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <variant>

#include "core/constants.h"

namespace barely {

struct EngineControlMessage {
  BarelyEngineControlType type = BarelyEngineControlType_kCount;
  float value = 0.0f;
};

struct EngineSeedMessage {
  int32_t seed = 0;
};

struct InstrumentCreateMessage {
  uint32_t instrument_index = kInvalidIndex;
};

struct InstrumentControlMessage {
  uint32_t instrument_index = kInvalidIndex;
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;
  float value;
};

struct NoteControlMessage {
  uint32_t note_index = kInvalidIndex;
  BarelyNoteControlType type = BarelyNoteControlType_kCount;
  float value = 0.0f;
};

struct NoteOffMessage {
  uint32_t note_index = kInvalidIndex;
};

struct NoteOnMessage {
  uint32_t note_index = kInvalidIndex;
  uint32_t instrument_index = kInvalidIndex;
  float pitch = 0.0f;
};

struct SampleDataMessage {
  uint32_t instrument_index = kInvalidIndex;
  uint32_t first_slice_index = kInvalidIndex;
};

using Message = std::variant<EngineControlMessage, EngineSeedMessage, InstrumentCreateMessage,
                             InstrumentControlMessage, NoteControlMessage, NoteOffMessage,
                             NoteOnMessage, SampleDataMessage>;

template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
