#include "barelymusician/composition/message_utils.h"

namespace barelyapi {

bool CompareMessage(const Message& lhs, const Message& rhs) {
  return lhs.timestamp < rhs.timestamp;
}

bool CompareTimestamp(const Message& message, int timestamp) {
  return message.timestamp < timestamp;
}

}  // namespace barelyapi
