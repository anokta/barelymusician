#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_

#include <variant>

namespace barelyapi {

// Note on data.
struct NoteOnData {
  // Note pitch.
  float pitch;

  // Note intensity.
  float intensity;
};

// Note off data.
struct NoteOffData {
  // Note pitch.
  float pitch;
};

// Message data type.
using MessageData = std::variant<NoteOnData, NoteOffData>;

// Message data visitor.
template <class... DataTypes>
struct MessageDataVisitor : DataTypes... {
  using DataTypes::operator()...;
};
template <class... DataTypes>
MessageDataVisitor(DataTypes...) -> MessageDataVisitor<DataTypes...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_DATA_H_
