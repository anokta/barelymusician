#include "engine/message_queue.h"

#include <cstdint>

#include "core/arena.h"
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

namespace barely {
namespace {

TEST(MessageQueueTest, AddSingleMessage) {
  const size_t size = GetMessageQueueSize();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  MessageQueue messages(arena);
  EXPECT_THAT(messages.GetNext(0), IsNull());
  EXPECT_THAT(messages.GetNext(1), IsNull());
  EXPECT_THAT(messages.GetNext(10), IsNull());

  messages.Add(1, InstrumentCreateMessage{5});
  EXPECT_THAT(messages.GetNext(0), IsNull());
  EXPECT_THAT(messages.GetNext(1), IsNull());
  EXPECT_THAT(
      messages.GetNext(10),
      AllOf(NotNull(), Pointee(Pair(1, VariantWith<InstrumentCreateMessage>(Field(
                                           &InstrumentCreateMessage::instrument_index, 5))))));

  // Message is already returned.
  EXPECT_THAT(messages.GetNext(10), IsNull());
}

TEST(MessageQueueTest, AddMultipleMessages) {
  const size_t size = GetMessageQueueSize();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  MessageQueue messages(arena);
  EXPECT_THAT(messages.GetNext(10), IsNull());

  for (uint32_t i = 0; i < 10; ++i) {
    messages.Add(i, InstrumentCreateMessage{i});
  }
  for (uint32_t i = 0; i < 10; ++i) {
    EXPECT_THAT(
        messages.GetNext(10),
        AllOf(NotNull(), Pointee(Pair(i, VariantWith<InstrumentCreateMessage>(Field(
                                             &InstrumentCreateMessage::instrument_index, i))))));
  }

  // All messages are already returned.
  EXPECT_THAT(messages.GetNext(10), IsNull());
}

}  // namespace
}  // namespace barely
