#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <cstddef>
#include <variant>
#include <vector>

namespace barely::internal {

/// Data message.
struct DataMessage {
  /// Data.
  std::vector<std::byte> data;
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
};

/// Parameter message.
struct ParameterMessage {
  /// Index.
  int index;

  /// Value.
  double value;

  /// Slope in value change per second.
  double slope;
};

/// Message alias.
using Message =
    std::variant<DataMessage, NoteOffMessage, NoteOnMessage, ParameterMessage>;

// Message visitor.
template <class... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <class... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
