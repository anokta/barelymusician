#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

#include "barelymusician/internal/sample_data.h"

namespace barely {

/// Control message.
struct ControlMessage {
  /// Identifier.
  int id;

  /// Value.
  double value;
};

/// Note control message.
struct NoteControlMessage {
  /// Pitch.
  double pitch;

  /// Identifier.
  int id;

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

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_H_
