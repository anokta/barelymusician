#include "barelymusician/message/message_buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Default number of samples to pop.
constexpr int kNumSamples = 16;

// Tests that the message buffer returns an added single message as expected.
TEST(MessageBufferTest, SinglePushPop) {
  const int kTimestamp = 10;

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push message.
  message_buffer.Push({{}, kTimestamp});
  EXPECT_FALSE(message_buffer.Empty());

  // Pop message.
  const auto iterator = message_buffer.GetIterator(kTimestamp, kNumSamples);
  EXPECT_NE(iterator.cbegin, iterator.cend);
  EXPECT_EQ(iterator.cbegin->timestamp, kTimestamp);

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
    message_buffer.Push({{}, i * kNumSamples});
    EXPECT_FALSE(message_buffer.Empty());
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const int timestamp = i * kNumSamples;
    const auto iterator = message_buffer.GetIterator(timestamp, kNumSamples);
    EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 1);
    EXPECT_EQ(iterator.cbegin->timestamp, timestamp);

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
    message_buffer.Push({{}, kTimestamp});
    EXPECT_FALSE(message_buffer.Empty());
  }

  // Pop all messages.
  const auto iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);
  for (auto it = iterator.cbegin; it != iterator.cend; ++it) {
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
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);

  for (int i = 0; i < kNumSamples; ++i) {
    message_buffer.Push({{}, i});
    EXPECT_FALSE(message_buffer.Empty());
  }
  iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumSamples);

  message_buffer.Clear();
  EXPECT_TRUE(message_buffer.Empty());

  iterator = message_buffer.GetIterator(0, kNumSamples);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);
}

}  // namespace
}  // namespace barelyapi
