#ifndef BARELYMUSICIAN_DSP_MESSAGE_H_
#define BARELYMUSICIAN_DSP_MESSAGE_H_

#include <barelymusician.h>

#include <array>
#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

#include "dsp/sample_data.h"

namespace barely {

/// Control message.
struct ControlMessage {
  /// Type.
  ControlType type;

  /// Value.
  float value;
};

/// Note control message.
struct NoteControlMessage {
  /// Pitch.
  float pitch;

  /// Type.
  NoteControlType type;

  /// Value.
  float value;
};

/// Note off message.
struct NoteOffMessage {
  /// Pitch.
  float pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Pitch.
  float pitch;

  /// Array of note controls.
  std::array<float, BarelyNoteControlType_kCount> controls;
};

/// Sample data message.
struct SampleDataMessage {
  /// Sample data.
  SampleData sample_data;
};

/// Message alias.
using Message = std::variant<ControlMessage, NoteControlMessage, NoteOffMessage, NoteOnMessage,
                             SampleDataMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_MESSAGE_H_
