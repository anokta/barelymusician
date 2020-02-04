#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_H_

#include <variant>

namespace barelyapi {

// Note on data.
struct NoteOnData {
  // Note index.
  float index;

  // Note intensity.
  float intensity;
};

// Note off data.
struct NoteOffData {
  // Note index.
  float index;
};

// Generic message with a position timestamp.
struct Message {
  // Message data.
  std::variant<NoteOnData, NoteOffData> data;

  // Message position.
  double position;
};

// Message visitor.
template <class... MessageDataTypes>
struct MessageVisitor : MessageDataTypes... {
  using MessageDataTypes::operator()...;
};
template <class... MessageDataTypes>
MessageVisitor(MessageDataTypes...)->MessageVisitor<MessageDataTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
