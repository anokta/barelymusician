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

// Tests that writing a structured data into a message is read back as expected.
TEST(MessageUtilsTest, WriteReadMessageData) {
  const TestMessageData kMessageData = {-12.5f, 20};

  Message message;
  WriteMessageData<TestMessageData>(kMessageData, message.data);

  const auto message_data = ReadMessageData<TestMessageData>(message.data);
  EXPECT_EQ(kMessageData.float_value, message_data.float_value);
  EXPECT_EQ(kMessageData.int_value, message_data.int_value);
}

}  // namespace
}  // namespace barelyapi
