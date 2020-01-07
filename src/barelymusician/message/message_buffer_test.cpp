#include "barelymusician/message/message_buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
constexpr int kNumSamples = 16;

// Test message ID.
constexpr int kMessageId = 1;

// Tests that the message buffer returns an added single message as expected.
TEST(MessageBufferTest, SinglePushPop) {
  const int kTimestamp = 10;

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push message.
  message_buffer.Push({kMessageId, {}, kTimestamp});
  EXPECT_FALSE(message_buffer.Empty());

  // Pop message.
  const auto iterator = message_buffer.GetIterator(kTimestamp, kNumSamples);
  EXPECT_NE(iterator.begin, iterator.end);
  EXPECT_EQ(iterator.begin->timestamp, kTimestamp);

  message_buffer.Clear(iterator);
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that the message buffer returns added messages as expected when asked
// one at a time.
TEST(MessageBufferTest, SingleMessagePerNumSamples) {
  const int kNumMessages = 5;

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push |kNumMessages| messages, each message to the beginning of each
  // |kNumSamples|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({kMessageId, {}, i * kNumSamples});
    EXPECT_FALSE(message_buffer.Empty());
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const int timestamp = i * kNumSamples;
    const auto iterator = message_buffer.GetIterator(timestamp, kNumSamples);
    EXPECT_EQ(std::distance(iterator.begin, iterator.end), 1);

    const Message& message = *iterator.begin;
    EXPECT_EQ(message.id, kMessageId);
    EXPECT_EQ(message.timestamp, timestamp);

    message_buffer.Clear(iterator);
  }
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that the message buffer returns added messages as expected when they
// have the same timestamps.
TEST(MessageBufferTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const int kTimestamp = 8;
  EXPECT_LE(kTimestamp, kNumSamples);

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({kMessageId, {}, kTimestamp});
    EXPECT_FALSE(message_buffer.Empty());
  }

  // Pop all messages.
  const auto iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), kNumMessages);
  for (auto it = iterator.begin; it != iterator.end; ++it) {
    EXPECT_EQ(it->id, kMessageId);
    EXPECT_EQ(it->timestamp, kTimestamp);
  }

  message_buffer.Clear(iterator);
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that clearing the queue removes all existing messages as expected.
TEST(MessageBufferTest, Clear) {
  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  MessageBuffer::Iterator iterator;
  iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), 0);

  for (int i = 0; i < kNumSamples; ++i) {
    message_buffer.Push({kMessageId, {}, i});
    EXPECT_FALSE(message_buffer.Empty());
  }
  iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), kNumSamples);

  message_buffer.Clear();
  EXPECT_TRUE(message_buffer.Empty());

  iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.begin, iterator.end), 0);
}

}  // namespace
}  // namespace barelyapi
