#include "barelymusician/message/message_buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the message buffer returns an added single message as expected.
TEST(MessageBufferTest, SingleMessage) {
  const double kTimestamp = 0.2;
  const double kBeginTimestamp = 0.1;
  const double kEndTimestamp = 0.5;
  EXPECT_GE(kTimestamp, kBeginTimestamp);
  EXPECT_LT(kTimestamp, kEndTimestamp);

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push message.
  message_buffer.Push({{}, kTimestamp});
  EXPECT_FALSE(message_buffer.Empty());

  // Pop message.

  const auto iterator =
      message_buffer.GetIterator(kBeginTimestamp, kEndTimestamp);
  EXPECT_NE(iterator.cbegin, iterator.cend);
  EXPECT_DOUBLE_EQ(iterator.cbegin->timestamp, kTimestamp);

  message_buffer.Clear(iterator);
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that the message buffer returns added messages as expected when asked
// one at a time.
TEST(MessageBufferTest, MultipleMessages) {
  const int kNumMessages = 5;

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push |kNumMessages| messages.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({{}, static_cast<double>(i)});
    EXPECT_FALSE(message_buffer.Empty());
  }
  // Pop one message at a time.
  for (int i = 0; i < kNumMessages; ++i) {
    const double begin_timestamp = static_cast<double>(i);
    const double end_timestamp = static_cast<double>(i + 1);
    const auto iterator =
        message_buffer.GetIterator(begin_timestamp, end_timestamp);
    EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 1);
    EXPECT_DOUBLE_EQ(iterator.cbegin->timestamp, begin_timestamp);

    message_buffer.Clear(iterator);
  }
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that the message buffer returns added messages as expected when they
// have the same timestamps.
TEST(MessageBufferTest, MultipleMessagesSameTimestamp) {
  const int kNumMessages = 4;
  const double kTimestamp = 2.4;
  const double kBeginTimestamp = 2.0;
  const double kEndTimestamp = 4.0;
  EXPECT_GE(kTimestamp, kBeginTimestamp);
  EXPECT_LT(kTimestamp, kEndTimestamp);

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push |kNumMessages| messages using the same |kTimestamp|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({{}, kTimestamp});
    EXPECT_FALSE(message_buffer.Empty());
  }

  // Pop all messages.
  const auto iterator =
      message_buffer.GetIterator(kBeginTimestamp, kEndTimestamp);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);
  for (auto it = iterator.cbegin; it != iterator.cend; ++it) {
    EXPECT_DOUBLE_EQ(it->timestamp, kTimestamp);
  }

  message_buffer.Clear(iterator);
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that clearing the queue removes all existing messages as expected.
TEST(MessageBufferTest, Clear) {
  const int kNumMessages = 10;

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  MessageBuffer::Iterator iterator;
  iterator = message_buffer.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);

  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push(
        {{}, static_cast<double>(i) / static_cast<double>(kNumMessages)});
    EXPECT_FALSE(message_buffer.Empty());
  }
  iterator = message_buffer.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);

  message_buffer.Clear();
  EXPECT_TRUE(message_buffer.Empty());

  iterator = message_buffer.GetIterator(0.0, 1.0);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 0);
}

}  // namespace
}  // namespace barelyapi
