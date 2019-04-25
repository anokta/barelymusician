#include "barelymusician/base/event.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that registering a single callback to event gets triggered as expected.
TEST(EventTest, RegisterSingleCallback) {
  const int kNumEventTriggers = 5;

  Event<int> event;

  int result = 0;
  const auto negate_callback = [&result](int value) { result = -value; };
  event.Register(negate_callback);

  for (int i = 0; i < kNumEventTriggers; ++i) {
    event.Trigger(i);
    EXPECT_EQ(-i, result);
  }
}

// Tests that registering multiple callbacks callback to event get triggered as
// expected.
TEST(EventTest, RegisterMultipleCallbacks) {
  const float kValue1 = 2.5f;
  const float kValue2 = -12.0f;

  Event<float, float> event;

  float sum = 0.0f;
  const auto sum_callback = [&sum](float value1, float value2) {
    sum = value1 + value2;
  };
  event.Register(sum_callback);

  float difference = 0.0f;
  const auto difference_callback = [&difference](float value1, float value2) {
    difference = value1 - value2;
  };
  event.Register(difference_callback);

  // Verify that triggering the event calls both callbacks.
  event.Trigger(kValue1, kValue2);
  EXPECT_FLOAT_EQ(kValue1 + kValue2, sum);
  EXPECT_FLOAT_EQ(kValue1 - kValue2, difference);
}

}  // namespace
}  // namespace barelyapi
