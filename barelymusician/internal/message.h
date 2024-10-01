#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

#include "barelymusician/internal/tuning.h"

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
  int pitch;

  /// Identifier.
  int id;

  /// Value.
  double value;
};

/// Note off message.
struct NoteOffMessage {
  /// Pitch.
  int pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Pitch.
  int pitch;

  /// Intensity.
  double intensity;
};

/// Tuning message.
struct TuningMessage {
  /// Optional tuning.
  std::optional<Tuning> tuning_or;
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
