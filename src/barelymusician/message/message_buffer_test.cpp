#include "barelymusician/message/message_buffer.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
const int kNumSamples = 16;

// Test message ID.
const int kMessageId = 1;

// Tests that the message buffer returns an added single message as expected.
TEST(MessageBufferTest, SinglePushPop) {
  const int kTimestamp = 10;
  EXPECT_LE(kTimestamp, kNumSamples);

  MessageBuffer message_buffer;

  Message message;
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));

  // Push message.
  message_buffer.Push({kMessageId, {0}, kTimestamp});

  // Pop message.
  EXPECT_TRUE(message_buffer.Pop(kNumSamples, &message));
  EXPECT_EQ(message.id, kMessageId);

  // Queue should be empty after pop.
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));
}

// Tests that the message buffer returns added messages as expected when asked
// one at a time.
TEST(MessageBufferTest, SingleMessagePerNumSamples) {
  const int kNumMessages = 5;

  MessageBuffer message_buffer;

  Message message;
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));

  // Push |kNumMessages| messages, each message to the beginning of each
  // |kNumSamples|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({kMessageId, {0}, i * kNumSamples});
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    EXPECT_TRUE(message_buffer.Pop(kNumSamples, &message));
    EXPECT_EQ(message.id, kMessageId);
    EXPECT_EQ(message.timestamp, 0);
    // There should be nothing left within the range after pop.
    EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));
    // Update timestamps.
    message_buffer.Update(kNumSamples);
  }
}

// Tests that the message buffer returns added messages as expected when they
// have the same timestamps.
TEST(MessageBufferTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const int kTimestamp = 8;
  EXPECT_LE(kTimestamp, kNumSamples);

  MessageBuffer message_buffer;

  Message message;
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({kMessageId, {0}, kTimestamp});
  }

  // Pop all messages.
  for (int i = 0; i < kNumMessages; ++i) {
    EXPECT_TRUE(message_buffer.Pop(kNumSamples, &message));
    EXPECT_EQ(message.id, kMessageId);
    EXPECT_EQ(message.timestamp, kTimestamp);
  }

  // Queue should be empty at the end.
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));
}

// Tests that clearing the queue removes all existing messages as expected.
TEST(MessageBufferTest, Clear) {
  MessageBuffer message_buffer;

  Message message;
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));

  for (int i = 0; i < kNumSamples; ++i) {
    message_buffer.Push({kMessageId, {0}, i});
  }

  message_buffer.Clear();
  EXPECT_FALSE(message_buffer.Pop(kNumSamples, &message));
}

}  // namespace
}  // namespace barelyapi
