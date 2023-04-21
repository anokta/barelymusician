#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <cstddef>
#include <variant>
#include <vector>

#include "barelymusician/engine/id.h"

namespace barely::internal {

/// Control message.
struct ControlMessage {
  /// Index.
  int index;

  /// Value.
  double value;

  /// Slope in value change per frame.
  double slope_per_frame;
};

/// Data message.
struct DataMessage {
  /// Data.
  std::vector<std::byte> data;
};

/// Effect control message.
struct EffectControlMessage {
  /// Effect identifier.
  Id effect_id;

  /// Index.
  int index;

  /// Value.
  double value;

  /// Slope in value change per frame.
  double slope_per_frame;
};

/// Effect data message.
struct EffectDataMessage {
  /// Effect identifier.
  Id effect_id;

  /// Data.
  std::vector<std::byte> data;
};

/// Note control message.
struct NoteControlMessage {
  /// Pitch.
  double pitch;

  /// Index.
  int index;

  /// Value.
  double value;

  /// Slope in value change per frame.
  double slope_per_frame;
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
using Message = std::variant<ControlMessage, DataMessage, EffectControlMessage,
                             EffectDataMessage, NoteControlMessage,
                             NoteOffMessage, NoteOnMessage>;

// Message visitor.
template <class... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <class... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
