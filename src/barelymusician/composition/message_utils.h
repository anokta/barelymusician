#ifndef BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_

#include <cstring>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"
#include "barelymusician/composition/message.h"

namespace barelyapi {

// Returns new |Message| with the given message |id|, |data| and |timestamp|.
//
// @param id Message ID.
// @param data Message data.
// @param timestamp Message timestamp.
// @return Message.
template <typename DataType>
Message BuildMessage(int id, const DataType& data, int timestamp);

// Compares the given two messages with respect to their timestamps.
//
// @param lhs First message.
// @param rhs Second message.
// @return True if the first message comes prior to the second message.
bool CompareMessage(const Message& lhs, const Message& rhs);

// Compares the given |message| against the given |timestamp|.
//
// @param message Message.
// @param timestamp Timestamp.
// @return True if the message comes prior to the timestamp.
bool CompareTimestamp(const Message& message, int timestamp);

// Reads the corresponding structured data for the given |message_data|.
//
// @param message_data Message data to be read.
// @return Structured data of |DataType|.
template <typename DataType>
DataType ReadMessageData(const unsigned char* message_data);

// Writes the given structured |data| into |message_data|.
//
// @param data Structured data of |DataType|.
// @param message_data Message data to be written into.
template <typename DataType>
void WriteMessageData(const DataType& data, unsigned char* message_data);

template <typename DataType>
Message BuildMessage(int id, const DataType& data, int timestamp) {
  Message message;
  message.id = id;
  WriteMessageData<DataType>(data, message.data);
  message.timestamp = timestamp;
  return message;
}

template <typename DataType>
DataType ReadMessageData(const unsigned char* message_data) {
  DCHECK(message_data);
  DataType data;
  DCHECK_LE(sizeof(data), kNumMaxMessageDataBytes);
  std::memcpy(&data, message_data, sizeof(data));
  return data;
}

template <typename DataType>
void WriteMessageData(const DataType& data, unsigned char* message_data) {
  DCHECK(message_data);
  DCHECK_LE(sizeof(data), kNumMaxMessageDataBytes);
  std::memcpy(message_data, &data, sizeof(data));
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_
