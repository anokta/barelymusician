#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <variant>

namespace barely {

struct EngineControlMessage {
  // Type.
  BarelyEngineControlType type = BarelyEngineControlType_kCount;

  // Value.
  float value = 0.0f;
};

struct EngineSeedMessage {
  // Seed.
  int32_t seed = 0;
};

struct InstrumentCreateMessage {
  // Instrument index;
  uint32_t instrument_index = UINT32_MAX;
};

struct InstrumentControlMessage {
  // Instrument index;
  uint32_t instrument_index = UINT32_MAX;

  // Type.
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;

  // Value.
  float value;
};

struct NoteControlMessage {
  // Note index.
  uint32_t note_index = UINT32_MAX;

  // Type.
  BarelyNoteControlType type = BarelyNoteControlType_kCount;

  // Value.
  float value = 0.0f;
};

struct NoteOffMessage {
  // Note index.
  uint32_t note_index = UINT32_MAX;
};

struct NoteOnMessage {
  // Note index.
  uint32_t note_index = UINT32_MAX;

  // Instrument index;
  uint32_t instrument_index = UINT32_MAX;

  // Pitch.
  float pitch = 0.0f;

  // Array of note controls.
  std::array<float, BarelyNoteControlType_kCount> controls = {};
};

struct SampleDataMessage {
  // Instrument index;
  uint32_t instrument_index = UINT32_MAX;

  // First slice index.
  uint32_t first_slice_index = UINT32_MAX;
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
