#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

namespace barely::internal {

/// Control message.
struct ControlMessage {
  /// Identifier.
  int id;

  /// Value.
  double value;
};

/// Data message.
struct DataMessage {
  /// Data.
  std::vector<std::byte> data;
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

/// Message alias.
using Message =
    std::variant<ControlMessage, DataMessage, NoteControlMessage, NoteOffMessage, NoteOnMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_H_
