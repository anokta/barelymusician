#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_H_

#include <variant>

#include "barelymusician/message/message_data.h"

namespace barelyapi {

// Generic message with a timestamp.
struct Message {
  // Message data.
  std::variant<NoteOnData, NoteOffData> data;

  // Message timestamp.
  double timestamp;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_H_
