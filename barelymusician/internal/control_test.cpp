#include "barelymusician/internal/control.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the control sets its value as expected.
TEST(ControlTest, Set) {
  Control control(ControlDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  EXPECT_TRUE(control.Set(12.0, 0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  // The control value is already set to 12.0.
  EXPECT_FALSE(control.Set(12.0, 0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  EXPECT_TRUE(control.Set(12.0, 1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 1.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Set(0.0, 1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 1.0);

  // The control value is already set to 0.0, which is clamped to 10.0.
  EXPECT_FALSE(control.Set(0.0, 1.0));
  EXPECT_FALSE(control.Set(10.0, 1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 1.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Set(50.0, 0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  // The control value is already set to 50.0, which is clamped to 20.0.
  EXPECT_FALSE(control.Set(50.0, 0.0));
  EXPECT_FALSE(control.Set(20.0, 0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  EXPECT_TRUE(control.Reset());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  // The control value is already reset.
  EXPECT_FALSE(control.Reset());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);
}

// Tests that the control updates its value as expected.
TEST(ControlTest, Update) {
  Control control(ControlDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  EXPECT_FALSE(control.Update(1.0));

  // Set the slope.
  EXPECT_TRUE(control.Set(15.0, 1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 1.0);

  EXPECT_TRUE(control.Update(1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 16.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 1.0);

  // Increase the slope.
  EXPECT_TRUE(control.Set(16.0, 2.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 16.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 2.0);

  EXPECT_TRUE(control.Update(1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 18.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 2.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Update(10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 2.0);

  EXPECT_FALSE(control.Update(10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 2.0);

  // Decrease the slope.
  EXPECT_TRUE(control.Set(20.0, -5.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), -5.0);

  EXPECT_TRUE(control.Update(1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), -5.0);

  // Decrease the slope to update beyond the minimum value.
  EXPECT_TRUE(control.Set(12.0, -10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), -10.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Update(1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), -10.0);

  EXPECT_FALSE(control.Update(10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), -10.0);

  // Reset the control.
  EXPECT_TRUE(control.Reset());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);

  EXPECT_FALSE(control.Update(1.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
  EXPECT_DOUBLE_EQ(control.GetSlopePerBeat(), 0.0);
}

// Tests that the controls are built from an array of control definitions as expected.
TEST(ControlTest, BuildControls) {
  const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{1.0},
      ControlDefinition{5.0},
  };

  const std::vector<Control> controls =
      BuildControls(control_definitions.data(), static_cast<int>(control_definitions.size()));
  ASSERT_EQ(controls.size(), 2);
  EXPECT_DOUBLE_EQ(controls[0].GetValue(), 1.0);
  EXPECT_DOUBLE_EQ(controls[1].GetValue(), 5.0);
}

}  // namespace
}  // namespace barely::internal
