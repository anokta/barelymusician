#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include "dsp/instrument_params.h"
#include "dsp/sample_data.h"

namespace barely {

/// Engine control message.
struct EngineControlMessage {
  /// Type.
  EngineControlType type;

  /// Value.
  float value;
};

/// Instrument control message.
struct InstrumentControlMessage {
  /// Instrument index;
  uint32_t instrument_index;

  /// Type.
  InstrumentControlType type;

  /// Value.
  float value;
};

/// Note control message.
struct NoteControlMessage {
  /// Instrument index;
  uint32_t instrument_index;

  /// Pitch.
  float pitch;

  /// Type.
  NoteControlType type;

  /// Value.
  float value;
};

/// Note off message.
struct NoteOffMessage {
  /// Instrument index;
  uint32_t instrument_index;

  /// Pitch.
  float pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Instrument index;
  uint32_t instrument_index;

  /// Pitch.
  float pitch;

  /// Array of note controls.
  std::array<float, BarelyNoteControlType_kCount> controls;
};

/// Sample data message.
struct SampleDataMessage {
  /// Instrument index;
  uint32_t instrument_index;

  /// Sample data.
  SampleData sample_data;
};

/// Message alias.
using Message = std::variant<EngineControlMessage, InstrumentControlMessage, NoteControlMessage,
                             NoteOffMessage, NoteOnMessage, SampleDataMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
