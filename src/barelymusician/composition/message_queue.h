#ifndef BARELYMUSICIAN_COMPOSITION_MESSAGE_QUEUE_H_
#define BARELYMUSICIAN_COMPOSITION_MESSAGE_QUEUE_H_

#include <list>
#include <vector>

#include "barelymusician/composition/message.h"

namespace barelyapi {

// Queue that receives and dispatches messages according to their timestamps.
class MessageQueue {
 public:
  // Pops the messages within the range from the queue.
  //
  // @param start_sample Start sample to begin.
  // @param num_samples Number of samples to end.
  // @return List of messages within the range.
  std::vector<Message> Pop(int start_sample, int num_samples);

  // Pushes a new message into the queue.
  //
  // @param message Message.
  void Push(const Message& message);

  // Resets the queue.
  void Reset();

 private:
  // Ordered message queue.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_MESSAGE_QUEUE_H_
