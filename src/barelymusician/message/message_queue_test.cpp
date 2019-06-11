#include "barelymusician/message/message_queue.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
const int kNumSamples = 16;

// Test message ID.
const int kMessageId = 1;

// Tests that the message queue returns an added single message as expected.
TEST(MessageQueueTest, SinglePushPop) {
  const int kTimestamp = 10;
  EXPECT_LE(kTimestamp, kNumSamples);

  MessageQueue message_queue;

  Message message;
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));

  // Push message.
  message_queue.Push({kMessageId, {0}, kTimestamp});

  // Pop message.
  EXPECT_TRUE(message_queue.Pop(kNumSamples, &message));
  EXPECT_EQ(message.id, kMessageId);

  // Queue should be empty after pop.
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));
}

// Tests that the message queue returns added messages as expected when asked
// one at a time.
TEST(MessageQueueTest, SingleMessagePerNumSamples) {
  const int kNumMessages = 5;

  MessageQueue message_queue;

  Message message;
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));

  // Push |kNumMessages| messages, each message to the beginning of each
  // |kNumSamples|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push({kMessageId, {0}, i * kNumSamples});
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    EXPECT_TRUE(message_queue.Pop(kNumSamples, &message));
    EXPECT_EQ(message.id, kMessageId);
    EXPECT_EQ(message.timestamp, 0);
    // There should be nothing left within the range after pop.
    EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));
    // Update timestamps.
    message_queue.Update(kNumSamples);
  }
}

// Tests that the message queue returns added messages as expected when they
// have the same timestamps.
TEST(MessageQueueTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const int kTimestamp = 8;
  EXPECT_LE(kTimestamp, kNumSamples);

  MessageQueue message_queue;

  Message message;
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push({kMessageId, {0}, kTimestamp});
  }

  // Pop all messages.
  for (int i = 0; i < kNumMessages; ++i) {
    EXPECT_TRUE(message_queue.Pop(kNumSamples, &message));
    EXPECT_EQ(message.id, kMessageId);
    EXPECT_EQ(message.timestamp, kTimestamp);
  }

  // Queue should be empty at the end.
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));
}

// Tests that clearing the queue removes all existing messages as expected.
TEST(MessageQueueTest, Clear) {
  MessageQueue message_queue;

  Message message;
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));

  for (int i = 0; i < kNumSamples; ++i) {
    message_queue.Push({kMessageId, {0}, i});
  }

  message_queue.Clear();
  EXPECT_FALSE(message_queue.Pop(kNumSamples, &message));
}

}  // namespace
}  // namespace barelyapi
