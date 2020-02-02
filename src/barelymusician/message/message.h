#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_H_

#include <variant>

#include "barelymusician/message/message_data.h"

namespace barelyapi {

// Generic message with a position timestamp.
struct Message {
  // Message data.
  std::variant<NoteOnData, NoteOffData> data;

  // Message position.
  double position;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_H_
