#include "barelymusician/internal/control.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the control sets its value as expected.
TEST(ControlTest, Set) {
  Control control(ControlDefinition{15, 10, 20});
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  EXPECT_TRUE(control.Set(12, 0));
  EXPECT_EQ(control.GetValue(), 12);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  // The control value is already set to 12.
  EXPECT_FALSE(control.Set(12, 0));
  EXPECT_EQ(control.GetValue(), 12);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  EXPECT_TRUE(control.Set(12, 1));
  EXPECT_EQ(control.GetValue(), 12);
  EXPECT_EQ(control.GetSlopePerBeat(), 1);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Set(0, 1));
  EXPECT_EQ(control.GetValue(), 10);
  EXPECT_EQ(control.GetSlopePerBeat(), 1);

  // The control value is already set to 0, which is clamped to 10.
  EXPECT_FALSE(control.Set(0, 1));
  EXPECT_FALSE(control.Set(10, 1));
  EXPECT_EQ(control.GetValue(), 10);
  EXPECT_EQ(control.GetSlopePerBeat(), 1);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Set(50, 0));
  EXPECT_EQ(control.GetValue(), 20);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  // The control value is already set to 50, which is clamped to 20.
  EXPECT_FALSE(control.Set(50, 0));
  EXPECT_FALSE(control.Set(20, 0));
  EXPECT_EQ(control.GetValue(), 20);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  EXPECT_TRUE(control.Reset());
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  // The control value is already reset.
  EXPECT_FALSE(control.Reset());
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);
}

// Tests that the control updates its value as expected.
TEST(ControlTest, Update) {
  Control control(ControlDefinition{15, 10, 20});
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  EXPECT_FALSE(control.Update(1));

  // Set the slope.
  EXPECT_TRUE(control.Set(15, 1));
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 1);

  EXPECT_TRUE(control.Update(1));
  EXPECT_EQ(control.GetValue(), 16);
  EXPECT_EQ(control.GetSlopePerBeat(), 1);

  // Increase the slope.
  EXPECT_TRUE(control.Set(16, 2));
  EXPECT_EQ(control.GetValue(), 16);
  EXPECT_EQ(control.GetSlopePerBeat(), 2);

  EXPECT_TRUE(control.Update(1));
  EXPECT_EQ(control.GetValue(), 18);
  EXPECT_EQ(control.GetSlopePerBeat(), 2);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Update(10));
  EXPECT_EQ(control.GetValue(), 20);
  EXPECT_EQ(control.GetSlopePerBeat(), 2);

  EXPECT_FALSE(control.Update(10));
  EXPECT_EQ(control.GetValue(), 20);
  EXPECT_EQ(control.GetSlopePerBeat(), 2);

  // Decrease the slope.
  EXPECT_TRUE(control.Set(20, -5));
  EXPECT_EQ(control.GetValue(), 20);
  EXPECT_EQ(control.GetSlopePerBeat(), -5);

  EXPECT_TRUE(control.Update(1));
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), -5);

  // Decrease the slope to update beyond the minimum value.
  EXPECT_TRUE(control.Set(12, -10));
  EXPECT_EQ(control.GetValue(), 12);
  EXPECT_EQ(control.GetSlopePerBeat(), -10);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Update(1));
  EXPECT_EQ(control.GetValue(), 10);
  EXPECT_EQ(control.GetSlopePerBeat(), -10);

  EXPECT_FALSE(control.Update(10));
  EXPECT_EQ(control.GetValue(), 10);
  EXPECT_EQ(control.GetSlopePerBeat(), -10);

  // Reset the control.
  EXPECT_TRUE(control.Reset());
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);

  EXPECT_FALSE(control.Update(1));
  EXPECT_EQ(control.GetValue(), 15);
  EXPECT_EQ(control.GetSlopePerBeat(), 0);
}

// Tests that the controls are built from an array of control definitions as expected.
TEST(ControlTest, BuildControls) {
  const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{1},
      ControlDefinition{5},
  };

  const std::vector<Control> controls =
      BuildControls(control_definitions.data(), static_cast<int>(control_definitions.size()));
  ASSERT_EQ(controls.size(), 2);
  EXPECT_EQ(controls[0].GetValue(), 1);
  EXPECT_EQ(controls[1].GetValue(), 5);
}

}  // namespace
}  // namespace barely::internal
