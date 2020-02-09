#ifndef BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_

#include <vector>

#include "barelymusician/engine/message.h"

namespace barelyapi {

// Message queue that receives and dispatches messages according to their
// positions.
class MessageQueue {
 public:
  // Message iterator.
  struct Iterator {
    // Iterator begin.
    std::vector<Message>::const_iterator cbegin;

    // Iterator end.
    std::vector<Message>::const_iterator cend;
  };

  // Clears the queue.
  void Clear();

  // Clears the queue within the given range.
  //
  // @param iterator Iterator range to clear the queue.
  void Clear(const Iterator& iterator);

  // Returns whether the queue is empty or not.
  //
  // @return True if empty.
  bool Empty() const;

  // Returns iterator within the given range.
  //
  // @param begin_position Start position.
  // @param end_position End position.
  Iterator GetIterator(double start_position, double end_position) const;

  // Pushes new message into the queue.
  //
  // @param position Message position.
  // @param data Message data.
  void Push(double position, const Message::Data& data);

 private:
  // Ordered message list.
  std::vector<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_MESSAGE_QUEUE_H_
