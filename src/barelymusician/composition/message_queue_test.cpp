#include "barelymusician/composition/message_queue.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
const int kNumSamples = 16;

// Test message type.
const MessageType kMessageType = MessageType::kNoteOn;

// Tests that the message queue returns an added single message as expected.
TEST(MessageQueueTest, SinglePushPop) {
  const int kTimestamp = 10;

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Pop(0, kNumSamples).empty());

  // Push single message.
  message_queue.Push({kMessageType, {0}, kTimestamp});
  EXPECT_TRUE(message_queue.Pop(0, kTimestamp).empty());

  const auto messages_in_range = message_queue.Pop(0, kTimestamp + kNumSamples);
  ASSERT_EQ(1, messages_in_range.size());
  EXPECT_EQ(kMessageType, messages_in_range.front().type);

  // Queue should be empty after pop.
  EXPECT_TRUE(message_queue.Pop(0, kTimestamp + kNumSamples).empty());
}

// Tests that the message queue returns added messages as expected when asked
// one at a time.
TEST(MessageQueueTest, SingleMessagePerNumSamples) {
  const int kNumMessages = 5;

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Pop(0, kNumSamples).empty());

  // Push |kNumMessages| messages, each message to the beginning of each
  // |kNumSamples|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push({kMessageType, {0}, i * kNumSamples});
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const int expected_timestamp = i * kNumSamples;
    const auto messages_in_range =
        message_queue.Pop(expected_timestamp, kNumSamples);
    ASSERT_EQ(1, messages_in_range.size());
    EXPECT_EQ(expected_timestamp, messages_in_range.front().timestamp);

    // There should be nothing left within the range after pop.
    EXPECT_TRUE(message_queue.Pop(expected_timestamp, kNumSamples).empty());
  }
}

// Tests that the message queue returns added messages as expected when they
// have the same timestamps.
TEST(MessageQueueTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const int kTimestamp = 8;

  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Pop(0, kNumSamples).empty());

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_queue.Push({kMessageType, {0}, kTimestamp});
  }
  EXPECT_TRUE(message_queue.Pop(0, kTimestamp).empty());

  const auto messages_in_range = message_queue.Pop(kTimestamp, kNumSamples);
  EXPECT_EQ(kNumMessages, messages_in_range.size());
  for (const auto& message : messages_in_range) {
    EXPECT_EQ(kTimestamp, message.timestamp);
  }

  // Queue should be empty after pop.
  EXPECT_TRUE(message_queue.Pop(kTimestamp, kNumSamples).empty());
}

// Tests that resetting the queue clears out the existing messages as expected.
TEST(MessageQueueTest, Reset) {
  MessageQueue message_queue;
  EXPECT_TRUE(message_queue.Pop(0, kNumSamples).empty());

  for (int i = 0; i < kNumSamples; ++i) {
    message_queue.Push({kMessageType, {0}, i});
  }

  message_queue.Reset();
  EXPECT_TRUE(message_queue.Pop(0, kNumSamples).empty());
}

}  // namespace
}  // namespace barelyapi
