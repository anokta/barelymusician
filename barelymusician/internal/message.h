#ifndef BARELYMUSICIAN_INTERNAL_MESSAGE_H_
#define BARELYMUSICIAN_INTERNAL_MESSAGE_H_

#include <cstddef>
#include <variant>
#include <vector>

#include "barelymusician/internal/effect.h"

namespace barely::internal {

/// Control message.
struct ControlMessage {
  /// Index.
  int index;

  /// Value.
  Rational value;

  /// Slope in value change per frame.
  Rational slope_per_frame;
};

/// Data message.
struct DataMessage {
  /// Data.
  std::vector<std::byte> data;
};

/// Effect control message.
struct EffectControlMessage {
  /// Effect.
  Effect* effect;

  /// Index.
  int index;

  /// Value.
  Rational value;

  /// Slope in value change per frame.
  Rational slope_per_frame;
};

/// Effect data message.
struct EffectDataMessage {
  /// Effect.
  Effect* effect;

  /// Data.
  std::vector<std::byte> data;
};

/// Note control message.
struct NoteControlMessage {
  /// Pitch.
  Rational pitch;

  /// Index.
  int index;

  /// Value.
  Rational value;

  /// Slope in value change per frame.
  Rational slope_per_frame;
};

/// Note off message.
struct NoteOffMessage {
  /// Pitch.
  Rational pitch;
};

/// Note on message.
struct NoteOnMessage {
  /// Pitch.
  Rational pitch;

  /// Intensity.
  float intensity;
};

/// Message alias.
using Message = std::variant<ControlMessage, DataMessage, EffectControlMessage, EffectDataMessage,
                             NoteControlMessage, NoteOffMessage, NoteOnMessage>;

// Message visitor.
template <typename... MessageTypes>
struct MessageVisitor : MessageTypes... {
  using MessageTypes::operator()...;
};
template <typename... MessageTypes>
MessageVisitor(MessageTypes...) -> MessageVisitor<MessageTypes...>;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_MESSAGE_H_
