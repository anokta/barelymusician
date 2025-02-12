#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

#include "barelymusician.h"
#include "dsp/sample_data.h"

namespace barely::internal {

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

  /// Intensity.
  float intensity;
};

/// Reference frequency message.
struct ReferenceFrequencyMessage {
  /// Reference frequency.
  float reference_frequency;
};

/// Sample data message.
struct SampleDataMessage {
  /// Sample data.
  SampleData sample_data;
};

/// Message alias.
using Message = std::variant<ControlMessage, NoteControlMessage, NoteOffMessage, NoteOnMessage,
                             ReferenceFrequencyMessage, SampleDataMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_H_
