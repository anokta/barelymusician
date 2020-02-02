#include "barelymusician/message/message_buffer.h"

#include <iterator>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the message buffer returns an added single message as expected.
TEST(MessageBufferTest, SingleMessage) {
  const double kPosition = 0.2;
  const double kStartPosition = 0.1;
  const double kEndPosition = 0.5;
  EXPECT_GE(kPosition, kStartPosition);
  EXPECT_LT(kPosition, kEndPosition);

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push message.
  message_buffer.Push({{}, kPosition});
  EXPECT_FALSE(message_buffer.Empty());

  // Pop message.

  const auto iterator =
      message_buffer.GetIterator(kStartPosition, kEndPosition);
  EXPECT_NE(iterator.cbegin, iterator.cend);
  EXPECT_DOUBLE_EQ(iterator.cbegin->position, kPosition);

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
    const double start_position = static_cast<double>(i);
    const double end_position = static_cast<double>(i + 1);
    const auto iterator =
        message_buffer.GetIterator(start_position, end_position);
    EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), 1);
    EXPECT_DOUBLE_EQ(iterator.cbegin->position, start_position);

    message_buffer.Clear(iterator);
  }
  EXPECT_TRUE(message_buffer.Empty());
}

// Tests that the message buffer returns added messages as expected when they
// have the same positions.
TEST(MessageBufferTest, MultipleMessagesSamePosition) {
  const int kNumMessages = 4;
  const double kPosition = 2.4;
  const double kStartPosition = 2.0;
  const double kEndPosition = 4.0;
  EXPECT_GE(kPosition, kStartPosition);
  EXPECT_LT(kPosition, kEndPosition);

  MessageBuffer message_buffer;
  EXPECT_TRUE(message_buffer.Empty());

  // Push |kNumMessages| messages using the same |kPosition|.
  for (int i = 0; i < kNumMessages; ++i) {
    message_buffer.Push({{}, kPosition});
    EXPECT_FALSE(message_buffer.Empty());
  }

  // Pop all messages.
  const auto iterator =
      message_buffer.GetIterator(kStartPosition, kEndPosition);
  EXPECT_EQ(std::distance(iterator.cbegin, iterator.cend), kNumMessages);
  for (auto it = iterator.cbegin; it != iterator.cend; ++it) {
    EXPECT_DOUBLE_EQ(it->position, kPosition);
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
