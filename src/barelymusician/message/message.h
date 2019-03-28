#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_H_

#include "barelymusician/base/constants.h"
#include "barelymusician/base/types.h"

namespace barelyapi {

// Message to perform.
struct Message {
  // Message ID.
  MessageId id;

  // Type-agnostic message data.
  Byte data[kNumMaxMessageDataBytes];

  // Message timestamp in samples.
  int timestamp;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_H_
