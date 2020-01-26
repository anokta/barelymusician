#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_BUFFER_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_BUFFER_H_

#include <vector>

#include "barelymusician/message/message.h"

namespace barelyapi {

// Message buffer that receives and dispatches messages according to their
// timestamps.
class MessageBuffer {
 public:
  // Buffer iterator.
  struct Iterator {
    // Iterator begin.
    std::vector<Message>::const_iterator cbegin;

    // Iterator end.
    std::vector<Message>::const_iterator cend;

    // Start timestamp.
    double timestamp;
  };

  // Clears the buffer.
  void Clear();

  // Clears the buffer within the given range.
  //
  // @param iterator Iterator range to clear the buffer.
  void Clear(const Iterator& iterator);

  // Returns whether the buffer is empty or not.
  //
  // @return True if empty.
  bool Empty() const;

  // Returns iterator within the given range.
  //
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp End timestamp.
  Iterator GetIterator(double begin_timestamp, double end_timestamp) const;

  // Pushes new message into the queue.
  //
  // @param message Message.
  void Push(const Message& message);

 private:
  // Ordered message queue.
  std::vector<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_BUFFER_H_
