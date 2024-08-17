#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
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
  /// Note.
  double note;

  /// Identifier.
  int id;

  /// Value.
  double value;
};

/// Note off message.
struct NoteOffMessage {
  /// Note.
  double note;
};

/// Note on message.
struct NoteOnMessage {
  /// Note.
  double note;

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
