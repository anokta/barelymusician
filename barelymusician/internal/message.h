#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"

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

/// Tuning message.
struct TuningMessage {
  // TODO(#137): This should probably be a pointer (same goes for `DataMessage`).
  TuningDefinition tuning;
};

/// Message alias.
using Message = std::variant<ControlMessage, DataMessage, NoteControlMessage, NoteOffMessage,
                             NoteOnMessage, TuningMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_H_
