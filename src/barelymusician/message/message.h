#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_H_

#include "barelymusician/base/constants.h"

namespace barelyapi {

// Generic message with a timestamp.
struct Message {
  // Message ID.
  int id;

  // Type-agnostic message data.
  unsigned char data[kNumMaxMessageDataBytes];

  // Message timestamp.
  int timestamp;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_H_
