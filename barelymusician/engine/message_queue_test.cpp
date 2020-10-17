#include "barelymusician/engine/message_queue.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the message queue returns an added single message as expected.
TEST(MessageQueueTest, SingleMessage) {
  const double kTimestamp = 0.2;
  const double kEndTimestamp = 0.5;
  EXPECT_LT(kTimestamp, kEndTimestamp);

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  // Push message.
  message_queue.Push(kTimestamp, {});
  EXPECT_FALSE(message_queue.Empty());

  // Pop message.
  const auto iterator = message_queue.GetIterator(kEndTimestamp);
  EXPECT_NE(iterator.cbegin, iterator.cend);
  EXPECT_DOUBLE_EQ(iterator.cbegin->timestamp, kTimestamp);

  message_queue.Clear(iterator);
  EXPECT_TRUE(message_queue.Empty());
}

// Tests that the message queue returns added messages as expected when asked
// one at a time.
TEST(MessageQueueTest, MultipleMessages) {
  const int kNumMessages = 5;

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  // Push |kNumMessages| messages.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push(static_cast<double>(i), {});
    EXPECT_FALSE(message_queue.Empty());
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const double begin_timestamp = static_cast<double>(i);
    const double end_timestamp = static_cast<double>(i + 1);
    const auto iterator = message_queue.GetIterator(end_timestamp);
    EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 1);
    EXPECT_DOUBLE_EQ(iterator.cbegin->timestamp, begin_timestamp);

    message_queue.Clear(iterator);
  }
  EXPECT_TRUE(message_queue.Empty());
}

// Tests that the message queue returns added messages as expected when they
// have the same timestamps.
TEST(MessageQueueTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const double kTimestamp = 2.4;
  const double kEndTimestamp = 4.0;
  EXPECT_LT(kTimestamp, kEndTimestamp);

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push(kTimestamp, {});
    EXPECT_FALSE(message_queue.Empty());
  }

  // Pop all messages.
  const auto iterator = message_queue.GetIterator(kEndTimestamp);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);
  for (auto it = iterator.cbegin; it != iterator.cend; ++it) {
    EXPECT_DOUBLE_EQ(it->timestamp, kTimestamp);
  }

  message_queue.Clear(iterator);
  EXPECT_TRUE(message_queue.Empty());
}

// Tests that clearing the queue removes all existing messages as expected.
TEST(MessageQueueTest, Clear) {
  const int kNumMessages = 10;

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  MessageQueue::Iterator iterator;
  iterator = message_queue.GetIterator(1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);

  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push(
        static_cast<double>(i) / static_cast<double>(kNumMessages), {});
    EXPECT_FALSE(message_queue.Empty());
  }
  iterator = message_queue.GetIterator(1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);

  message_queue.Clear();
  EXPECT_TRUE(message_queue.Empty());

  iterator = message_queue.GetIterator(1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);
}

}  // namespace
}  // namespace barelyapi
