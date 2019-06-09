#include "barelymusician/composition/message_utils.h"

#include "barelymusician/composition/message.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Test message data.
struct TestMessageData {
  float float_value;
  int int_value;
};

// Tests that building new message populates the fields as expected.
TEST(MessageUtilsTest, BuildMessage) {
  const int kId = 1;
  const TestMessageData kData = {3.0f, -6};
  const int kTimestamp = 512;

  const Message message = BuildMessage<TestMessageData>(kId, kData, kTimestamp);
  EXPECT_EQ(message.id, kId);
  const auto message_data = ReadMessageData<TestMessageData>(message.data);
  EXPECT_FLOAT_EQ(message_data.float_value, kData.float_value);
  EXPECT_EQ(message_data.int_value, kData.int_value);
  EXPECT_EQ(message.timestamp, kTimestamp);
}

// Tests that messages are compared as expected according to their timestamps.
TEST(MessageUtilsTest, CompareMessageTimestamp) {
  const int kFirstTimestamp = 10;
  const int kSecondTimestamp = 32;

  Message first_message;
  first_message.timestamp = kFirstTimestamp;
  Message second_message;
  second_message.timestamp = kSecondTimestamp;

  EXPECT_TRUE(CompareMessage(first_message, second_message));
  EXPECT_TRUE(CompareTimestamp(first_message, kSecondTimestamp));

  EXPECT_FALSE(CompareMessage(second_message, first_message));
  EXPECT_FALSE(CompareTimestamp(second_message, kFirstTimestamp));
}

// Tests that writing a structured data into a message is read back as expected.
TEST(MessageUtilsTest, WriteReadMessageData) {
  const TestMessageData kData = {-12.5f, 20};

  Message message;
  WriteMessageData<TestMessageData>(kData, message.data);

  const auto message_data = ReadMessageData<TestMessageData>(message.data);
  EXPECT_FLOAT_EQ(message_data.float_value, kData.float_value);
  EXPECT_EQ(message_data.int_value, kData.int_value);
}

}  // namespace
}  // namespace barelyapi
