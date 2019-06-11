#ifndef BARELYMUSICIAN_MESSAGE_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_MESSAGE_MESSAGE_QUEUE_H_

#include <deque>

#include "barelymusician/message/message.h"

namespace barelyapi {

// Queue that receives and dispatches messages according to their timestamps.
class MessageQueue {
 public:
  // Clears the queue.
  void Clear();

  // Pops next message from the queue.
  //
  // @param num_samples Maximum number of samples allowed for message timestamp.
  // @param message Message to be written into.
  // @return True if successful.
  bool Pop(int num_samples, Message* message);

  // Pushes new message into the queue.
  //
  // @param message Message.
  void Push(const Message& message);

  // Updates the message timestamps in the queue.
  //
  // @param num_samples Number of samples to iterate.
  void Update(int num_samples);

 private:
  // Ordered message queue.
  std::deque<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_MESSAGE_MESSAGE_QUEUE_H_
