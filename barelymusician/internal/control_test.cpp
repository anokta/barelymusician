#include "barelymusician/internal/control.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that the control sets its value as expected.
TEST(ControlTest, Set) {
  int callback_count = 0;
  Control control(ControlDefinition{0, 15.0, 10.0, 20.0},
                  [&](int /*index*/, double /*value*/) { ++callback_count; });
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  control.SetValue(12.0);
  EXPECT_EQ(callback_count, 1);
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // The control value is already set to 12.0.
  control.SetValue(12.0);
  EXPECT_EQ(callback_count, 1);
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // Verify that the control value is clamped at the minimum value.
  control.SetValue(0.0);
  EXPECT_EQ(callback_count, 2);
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // The control value is already set to 0.0, which is clamped to 10.0.
  control.SetValue(0.0);
  EXPECT_EQ(callback_count, 2);
  control.SetValue(10.0);
  EXPECT_EQ(callback_count, 2);
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // Verify that the control value is clamped at the maximum value.
  control.SetValue(50.0);
  EXPECT_EQ(callback_count, 3);
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  // The control value is already set to 50.0, which is clamped to 20.0.
  control.SetValue(50.0);
  EXPECT_EQ(callback_count, 3);
  control.SetValue(20.0);
  EXPECT_EQ(callback_count, 3);
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  control.ResetValue();
  EXPECT_EQ(callback_count, 4);
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  // The control value is already reset.
  control.ResetValue();
  EXPECT_EQ(callback_count, 4);
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
}

// Tests that the array of controls is built from an array of control definitions as expected.
TEST(ControlTest, BuildControls) {
  const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{0, 1.0},
      ControlDefinition{1, 5.0},
  };

  const auto controls = BuildControls(control_definitions, [](int /*index*/, double /*value*/) {});
  ASSERT_EQ(controls.size(), 2);
  EXPECT_DOUBLE_EQ(controls[0].GetValue(), 1.0);
  EXPECT_DOUBLE_EQ(controls[1].GetValue(), 5.0);
}

}  // namespace
}  // namespace barely
