#include "barelymusician/engine/param.h"

#include "barelymusician/engine/param_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the parameter sets its value as expected.
TEST(ParamTest, SetValue) {
  Param param(ParamDefinition{1, -2.0f});
  EXPECT_FLOAT_EQ(param.GetValue(), -2.0f);

  EXPECT_TRUE(param.SetValue(0.5f));
  EXPECT_FLOAT_EQ(param.GetValue(), 0.5f);

  // The parameter value is already set to 0.5f.
  EXPECT_FALSE(param.SetValue(0.5f));
  EXPECT_FLOAT_EQ(param.GetValue(), 0.5f);

  EXPECT_TRUE(param.ResetValue());
  EXPECT_FLOAT_EQ(param.GetValue(), -2.0f);

  // The parameter value is already reset.
  EXPECT_FALSE(param.ResetValue());
  EXPECT_FLOAT_EQ(param.GetValue(), -2.0f);
}

// Tests that the parameter sets its value with respect to its minimum and
// maximum boundaries as expected.
TEST(ParamTest, SetValueMinMax) {
  Param param(ParamDefinition{2, 5.0f, 10.0f, 20.0f});

  // Verify that the default value is also clamped at the minimum value.
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  EXPECT_TRUE(param.SetValue(12.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 12.0f);

  // The parameter value is already set to 12.0f.
  EXPECT_FALSE(param.SetValue(12.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 12.0f);

  // Verify that the parameter value is clamped at the minimum value.
  EXPECT_TRUE(param.SetValue(0.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  // The parameter value is already set to 0.0f, which is clamped to 10.0f.
  EXPECT_FALSE(param.SetValue(0.0f));
  EXPECT_FALSE(param.SetValue(10.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  // Verify that the parameter value is clamped at the maximum value.
  EXPECT_TRUE(param.SetValue(50.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 20.0f);

  // The parameter value is already set to 50.0f, which is clamped to 20.0f.
  EXPECT_FALSE(param.SetValue(50.0f));
  EXPECT_FALSE(param.SetValue(20.0f));
  EXPECT_FLOAT_EQ(param.GetValue(), 20.0f);

  EXPECT_TRUE(param.ResetValue());
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  // The parameter value is already reset.
  EXPECT_FALSE(param.ResetValue());
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);
}

}  // namespace
}  // namespace barelyapi
