#ifndef BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_

#include <cstring>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

// Reads the corresponding structured data for the given |message_data|.
//
// @param message_data Message data to be read.
// @return Structured data of |DataType|.
template <typename DataType>
DataType ReadMessageData(const unsigned char* message_data) {
  DataType data;
  DCHECK_LE(sizeof(data), kNumMaxMessageDataBytes);
  memcpy(&data, message_data, sizeof(data));
  return data;
}

// Writes the given structured |data| into |message_data|.
//
// @param data Structured data of |DataType|.
// @param message_data Message data to be written into.
template <typename DataType>
void WriteMessageData(const DataType& data, unsigned char* message_data) {
  DCHECK_LE(sizeof(data), kNumMaxMessageDataBytes);
  memcpy(message_data, &data, sizeof(data));
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_MESSAGE_UTILS_H_
