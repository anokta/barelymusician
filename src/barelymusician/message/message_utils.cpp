#include "barelymusician/message/message_utils.h"

namespace barelyapi {

bool CompareMessage(const Message& lhs, const Message& rhs) {
  return lhs.timestamp < rhs.timestamp;
}

}  // namespace barelyapi
