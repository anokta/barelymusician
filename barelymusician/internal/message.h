#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

#include "barelymusician.h"
#include "barelymusician/internal/sample_data.h"

namespace barely::internal {

/// Control message.
struct ControlMessage {
  /// Type.
  ControlType type;

  /// Value.
  double value;
};

/// Note control message.
struct NoteControlMessage {
  /// Pitch.
  double pitch;

  /// Type.
  NoteControlType type;

  /// Value.
  double value;
};

/// Note off message.
struct NoteOffMessage {
  /// Pitch.
  double pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Pitch.
  double pitch;

  /// Intensity.
  double intensity;
};

/// Reference frequency message.
struct ReferenceFrequencyMessage {
  /// Reference frequency.
  double reference_frequency;
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
