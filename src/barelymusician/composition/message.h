#ifndef BARELYMUSICIAN_COMPOSITION_MESSAGE_H_
#define BARELYMUSICIAN_COMPOSITION_MESSAGE_H_

#include "barelymusician/base/constants.h"

namespace barelyapi {

// Message type.
enum class MessageType {
  kNoteOn,      // Note on message.
  kNoteOff,     // Note off message.
  kFloatParam,  // Float parameter message.
};

// Message to perform.
struct Message {
  // Message type.
  MessageType type;

  // Type-agnostic message data.
  unsigned char data[kNumMaxMessageDataBytes];

  // Message timestamp in samples.
  int timestamp;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_MESSAGE_H_
