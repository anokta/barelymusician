#include "engine/message_queue.h"

#include "engine/message.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

using ::testing::AllOf;
using ::testing::Field;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Pointee;
using ::testing::VariantWith;

namespace barely::internal {
namespace {

// Tests that adding a single message is queued as expected.
TEST(MessageQueueTest, AddSingleMessage) {
  MessageQueue messages;
  EXPECT_THAT(messages.GetNext(0), IsNull());
  EXPECT_THAT(messages.GetNext(1), IsNull());
  EXPECT_THAT(messages.GetNext(10), IsNull());

  messages.Add(1, NoteOffMessage{5.0f});
  EXPECT_THAT(messages.GetNext(0), IsNull());
  EXPECT_THAT(messages.GetNext(1), IsNull());
  EXPECT_THAT(
      messages.GetNext(10),
      AllOf(NotNull(),
            Pointee(Pair(1, VariantWith<NoteOffMessage>(Field(&NoteOffMessage::pitch, 5.0f))))));

  // Message is already returned.
  EXPECT_THAT(messages.GetNext(10), IsNull());
}

// Tests that adding multiple messages are queued as expected.
TEST(MessageQueueTest, AddMultipleMessages) {
  MessageQueue messages;
  EXPECT_THAT(messages.GetNext(10), IsNull());

  for (int i = 0; i < 10; ++i) {
    messages.Add(i, NoteOffMessage{static_cast<float>(i)});
  }
  for (int i = 0; i < 10; ++i) {
    EXPECT_THAT(
        messages.GetNext(10),
        AllOf(NotNull(), Pointee(Pair(i, VariantWith<NoteOffMessage>(Field(
                                             &NoteOffMessage::pitch, static_cast<float>(i)))))));
  }

  // All messages are already returned.
  EXPECT_THAT(messages.GetNext(10), IsNull());
}

// TODO(#97): Add more tests with concurrency.

}  // namespace
}  // namespace barely::internal
