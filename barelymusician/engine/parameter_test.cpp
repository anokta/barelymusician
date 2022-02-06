#include "barelymusician/engine/parameter.h"

#include "barelymusician/engine/parameter_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the parameter sets its value as expected.
TEST(ParameterTest, SetValue) {
  Parameter parameter(ParameterDefinition{-2.0f});
  EXPECT_FLOAT_EQ(parameter.GetValue(), -2.0f);

  EXPECT_TRUE(parameter.SetValue(0.5f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 0.5f);

  // The parameter value is already set to 0.5f.
  EXPECT_FALSE(parameter.SetValue(0.5f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 0.5f);

  EXPECT_TRUE(parameter.ResetValue());
  EXPECT_FLOAT_EQ(parameter.GetValue(), -2.0f);

  // The parameter value is already reset.
  EXPECT_FALSE(parameter.ResetValue());
  EXPECT_FLOAT_EQ(parameter.GetValue(), -2.0f);
}

// Tests that the parameter sets its value with respect to its minimum and
// maximum boundaries as expected.
TEST(ParameterTest, SetValueMinMax) {
  Parameter parameter(ParameterDefinition{15.0f, 10.0f, 20.0f});
  EXPECT_FLOAT_EQ(parameter.GetValue(), 15.0f);

  EXPECT_TRUE(parameter.SetValue(12.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 12.0f);

  // The parameter value is already set to 12.0f.
  EXPECT_FALSE(parameter.SetValue(12.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 12.0f);

  // Verify that the parameter value is clamped at the minimum value.
  EXPECT_TRUE(parameter.SetValue(0.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 10.0f);

  // The parameter value is already set to 0.0f, which is clamped to 10.0f.
  EXPECT_FALSE(parameter.SetValue(0.0f));
  EXPECT_FALSE(parameter.SetValue(10.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 10.0f);

  // Verify that the parameter value is clamped at the maximum value.
  EXPECT_TRUE(parameter.SetValue(50.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 20.0f);

  // The parameter value is already set to 50.0f, which is clamped to 20.0f.
  EXPECT_FALSE(parameter.SetValue(50.0f));
  EXPECT_FALSE(parameter.SetValue(20.0f));
  EXPECT_FLOAT_EQ(parameter.GetValue(), 20.0f);

  EXPECT_TRUE(parameter.ResetValue());
  EXPECT_FLOAT_EQ(parameter.GetValue(), 15.0f);

  // The parameter value is already reset.
  EXPECT_FALSE(parameter.ResetValue());
  EXPECT_FLOAT_EQ(parameter.GetValue(), 15.0f);
}

}  // namespace
}  // namespace barelyapi
