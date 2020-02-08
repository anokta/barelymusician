#include "barelymusician/engine/message_queue.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the message queue returns an added single message as expected.
TEST(MessageQueueTest, SingleMessage) {
  const double kPosition = 0.2;
  const double kStartPosition = 0.1;
  const double kEndPosition = 0.5;
  EXPECT_GE(kPosition, kStartPosition);
  EXPECT_LT(kPosition, kEndPosition);

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  // Push message.
  message_queue.Push({kPosition});
  EXPECT_FALSE(message_queue.Empty());

  // Pop message.
  const auto iterator = message_queue.GetIterator(kStartPosition, kEndPosition);
  EXPECT_NE(iterator.cbegin, iterator.cend);
  EXPECT_DOUBLE_EQ(iterator.cbegin->position, kPosition);

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
    message_queue.Push({static_cast<double>(i)});
    EXPECT_FALSE(message_queue.Empty());
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const double start_position = static_cast<double>(i);
    const double end_position = static_cast<double>(i + 1);
    const auto iterator =
        message_queue.GetIterator(start_position, end_position);
    EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 1);
    EXPECT_DOUBLE_EQ(iterator.cbegin->position, start_position);

    message_queue.Clear(iterator);
  }
  EXPECT_TRUE(message_queue.Empty());
}

// Tests that the message queue returns added messages as expected when they
// have the same positions.
TEST(MessageQueueTest, MultipleMessagesSamePosition) {
  const int kNumMessages = 4;
  const double kPosition = 2.4;
  const double kStartPosition = 2.0;
  const double kEndPosition = 4.0;
  EXPECT_GE(kPosition, kStartPosition);
  EXPECT_LT(kPosition, kEndPosition);

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Empty());

  // Push |kNumMessages| messages using the same |kPosition|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push({kPosition});
    EXPECT_FALSE(message_queue.Empty());
  }

  // Pop all messages.
  const auto iterator = message_queue.GetIterator(kStartPosition, kEndPosition);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);
  for (auto it = iterator.cbegin; it != iterator.cend; ++it) {
    EXPECT_DOUBLE_EQ(it->position, kPosition);
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
  iterator = message_queue.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);

  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push(
        {static_cast<double>(i) / static_cast<double>(kNumMessages)});
    EXPECT_FALSE(message_queue.Empty());
  }
  iterator = message_queue.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);

  message_queue.Clear();
  EXPECT_TRUE(message_queue.Empty());

  iterator = message_queue.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);
}

}  // namespace
}  // namespace barelyapi
