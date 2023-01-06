#include "barelymusician/engine/control.h"

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that control sets its value as expected.
TEST(ControlTest, SetValue) {
  Control control(ControlDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  EXPECT_TRUE(control.SetValue(12.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // The control value is already set to 12.0.
  EXPECT_FALSE(control.SetValue(12.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.SetValue(0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // The control value is already set to 0.0, which is clamped to 10.0.
  EXPECT_FALSE(control.SetValue(0.0));
  EXPECT_FALSE(control.SetValue(10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.SetValue(50.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  // The control value is already set to 50.0, which is clamped to 20.0.
  EXPECT_FALSE(control.SetValue(50.0));
  EXPECT_FALSE(control.SetValue(20.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  EXPECT_TRUE(control.ResetValue());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  // The control value is already reset.
  EXPECT_FALSE(control.ResetValue());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
}

}  // namespace
}  // namespace barely::internal
