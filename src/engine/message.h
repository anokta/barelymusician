#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <variant>

namespace barely {

struct EngineControlMessage {
  BarelyEngineControlType type = BarelyEngineControlType_kCount;
  float value = 0.0f;
};

struct EngineSeedMessage {
  int32_t seed = 0;
};

struct InstrumentCreateMessage {
  uint32_t instrument_index = UINT32_MAX;
};

struct InstrumentControlMessage {
  uint32_t instrument_index = UINT32_MAX;
  BarelyInstrumentControlType type = BarelyInstrumentControlType_kCount;
  float value;
};

struct NoteControlMessage {
  uint32_t note_index = UINT32_MAX;
  BarelyNoteControlType type = BarelyNoteControlType_kCount;
  float value = 0.0f;
};

struct NoteOffMessage {
  uint32_t note_index = UINT32_MAX;
};

struct NoteOnMessage {
  uint32_t note_index = UINT32_MAX;
  uint32_t instrument_index = UINT32_MAX;
  float pitch = 0.0f;
  std::array<float, BarelyNoteControlType_kCount> controls = {/*kGain=*/1.0f, /*kPitchShift=*/0.0f};
};

struct SampleDataMessage {
  uint32_t instrument_index = UINT32_MAX;
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
