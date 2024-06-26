#include "barelymusician/internal/control.h"

#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that the control sets its value as expected.
TEST(ControlTest, Set) {
  Control control(ControlDefinition{0, 15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  EXPECT_TRUE(control.Set(12.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // The control value is already set to 12.0.
  EXPECT_FALSE(control.Set(12.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 12.0);

  // Verify that the control value is clamped at the minimum value.
  EXPECT_TRUE(control.Set(0.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // The control value is already set to 0.0, which is clamped to 10.0.
  EXPECT_FALSE(control.Set(0.0));
  EXPECT_FALSE(control.Set(10.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 10.0);

  // Verify that the control value is clamped at the maximum value.
  EXPECT_TRUE(control.Set(50.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  // The control value is already set to 50.0, which is clamped to 20.0.
  EXPECT_FALSE(control.Set(50.0));
  EXPECT_FALSE(control.Set(20.0));
  EXPECT_DOUBLE_EQ(control.GetValue(), 20.0);

  EXPECT_TRUE(control.Reset());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);

  // The control value is already reset.
  EXPECT_FALSE(control.Reset());
  EXPECT_DOUBLE_EQ(control.GetValue(), 15.0);
}

// Tests that the controls are built from an array of control definitions as expected.
TEST(ControlTest, BuildControls) {
  const std::vector<ControlDefinition> control_definitions = {
      ControlDefinition{2, 1.0},
      ControlDefinition{10, 5.0},
  };

  const std::unordered_map<int, Control> controls =
      BuildControls(control_definitions.data(), static_cast<int>(control_definitions.size()));
  ASSERT_EQ(controls.size(), 2);
  EXPECT_DOUBLE_EQ(controls.find(2)->second.GetValue(), 1.0);
  EXPECT_DOUBLE_EQ(controls.find(10)->second.GetValue(), 5.0);
}

}  // namespace
}  // namespace barely::internal
