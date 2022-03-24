#include "barelymusician/engine/parameter.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that parameter sets its value as expected.
TEST(ParameterTest, SetValue) {
  Parameter parameter(ParameterDefinition{15.0, 10.0, 20.0});
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 15.0);

  EXPECT_TRUE(parameter.SetValue(12.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 12.0);

  // The parameter value is already set to 12.0.
  EXPECT_FALSE(parameter.SetValue(12.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 12.0);

  // Verify that the parameter value is clamped at the minimum value.
  EXPECT_TRUE(parameter.SetValue(0.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 10.0);

  // The parameter value is already set to 0.0, which is clamped to 10.0.
  EXPECT_FALSE(parameter.SetValue(0.0));
  EXPECT_FALSE(parameter.SetValue(10.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 10.0);

  // Verify that the parameter value is clamped at the maximum value.
  EXPECT_TRUE(parameter.SetValue(50.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 20.0);

  // The parameter value is already set to 50.0, which is clamped to 20.0.
  EXPECT_FALSE(parameter.SetValue(50.0));
  EXPECT_FALSE(parameter.SetValue(20.0));
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 20.0);

  EXPECT_TRUE(parameter.ResetValue());
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 15.0);

  // The parameter value is already reset.
  EXPECT_FALSE(parameter.ResetValue());
  EXPECT_DOUBLE_EQ(parameter.GetValue(), 15.0);
}

}  // namespace
}  // namespace barelyapi
