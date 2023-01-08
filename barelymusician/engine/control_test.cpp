#include "barelymusician/engine/control.h"

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that control sets its value as expected.
TEST(ControlTest, Set) {
  Control control(ControlDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_TRUE(control.Set(12.0, 0.0));
  EXPECT_DOUBLE_EQ(control.Get(), 12.0);

  // The control value is already set to 12.0.
  EXPECT_FALSE(control.Set(12.0, 0.0));
  EXPECT_DOUBLE_EQ(control.Get(), 12.0);

  EXPECT_TRUE(control.Set(12.0, 1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 12.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Set(0.0, 1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 10.0);

  // The control value is already set to 0.0, which is clamped to 10.0.
  EXPECT_FALSE(control.Set(0.0, 1.0));
  EXPECT_FALSE(control.Set(10.0, 1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 10.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Set(50.0, 0.0));
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  // The control value is already set to 50.0, which is clamped to 20.0.
  EXPECT_FALSE(control.Set(50.0, 0.0));
  EXPECT_FALSE(control.Set(20.0, 0.0));
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  EXPECT_TRUE(control.Reset());
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  // The control value is already reset.
  EXPECT_FALSE(control.Reset());
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);
}

// Tests that control updates its value as expected.
TEST(ControlTest, UpdateBy) {
  Control control(ControlDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_FALSE(control.UpdateBy(1.0));

  // Set the slope.
  EXPECT_TRUE(control.Set(15.0, 1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_TRUE(control.UpdateBy(1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 16.0);

  // Increase the slope.
  EXPECT_TRUE(control.Set(16.0, 2.0));
  EXPECT_DOUBLE_EQ(control.Get(), 16.0);

  EXPECT_TRUE(control.UpdateBy(1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 18.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.UpdateBy(10.0));
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  EXPECT_FALSE(control.UpdateBy(10.0));
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  // Decrease the slope.
  EXPECT_TRUE(control.Set(20.0, -5.0));
  EXPECT_DOUBLE_EQ(control.Get(), 20.0);

  EXPECT_TRUE(control.UpdateBy(1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  // Decrease the slope to update beyond the minimum value.
  EXPECT_TRUE(control.Set(12.0, -10.0));
  EXPECT_DOUBLE_EQ(control.Get(), 12.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.UpdateBy(1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 10.0);

  EXPECT_FALSE(control.UpdateBy(10.0));
  EXPECT_DOUBLE_EQ(control.Get(), 10.0);

  // Reset the control.
  EXPECT_TRUE(control.Reset());
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);

  EXPECT_FALSE(control.UpdateBy(1.0));
  EXPECT_DOUBLE_EQ(control.Get(), 15.0);
}

}  // namespace
}  // namespace barely::internal
