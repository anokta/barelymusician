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

// Generic message with a timestamp.
struct Message {
  // Message data signature.
  using Data = std::variant<NoteOnData, NoteOffData>;

  // Message timestamp.
  double timestamp;

  // Message data.
  Data data;
};

// Message visitor.
template <class... MessageDataTypes>
struct MessageVisitor : MessageDataTypes... {
  using MessageDataTypes::operator()...;
};
template <class... MessageDataTypes>
MessageVisitor(MessageDataTypes...) -> MessageVisitor<MessageDataTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_H_
