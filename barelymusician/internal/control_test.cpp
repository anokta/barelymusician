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
                  [&](int /*id*/, double /*value*/) { ++callback_count; });
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

// Tests that the control map is built from an array of control definitions as expected.
TEST(ControlTest, BuildControlMap) {
  const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{2, 1.0},
      ControlDefinition{10, 5.0},
  };

  const ControlMap control_map =
      BuildControlMap(control_definitions, [](int /*id*/, double /*value*/) {});
  ASSERT_EQ(control_map.size(), 2);
  EXPECT_DOUBLE_EQ(control_map.find(2)->second.GetValue(), 1.0);
  EXPECT_DOUBLE_EQ(control_map.find(10)->second.GetValue(), 5.0);
}

}  // namespace
}  // namespace barely
