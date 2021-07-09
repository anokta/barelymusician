#include "barelymusician/engine/instrument_param.h"

#include "barelymusician/engine/instrument_param_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the instrument parameter sets its value as expected.
TEST(InstrumentParamTest, SetValue) {
  InstrumentParam param(InstrumentParamDefinition{1, -2.0f});
  EXPECT_FLOAT_EQ(param.GetValue(), -2.0f);

  param.SetValue(0.5f);
  EXPECT_FLOAT_EQ(param.GetValue(), 0.5f);

  param.ResetValue();
  EXPECT_FLOAT_EQ(param.GetValue(), -2.0f);
}

// Tests that the instrument parameter sets its value with respect to its
// minimum and maximum boundaries as expected.
TEST(InstrumentParamTest, SetValueMinMax) {
  InstrumentParam param(InstrumentParamDefinition{2, 5.0f, 10.0f, 20.0f});

  // Verify that the default value is also clamped at the minimum value.
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  param.SetValue(12.0f);
  EXPECT_FLOAT_EQ(param.GetValue(), 12.0f);

  // Verify that the parameter value is clamped at the minimum value.
  param.SetValue(0.0f);
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);

  // Verify that the parameter value is clamped at the maximum value.
  param.SetValue(50.0f);
  EXPECT_FLOAT_EQ(param.GetValue(), 20.0f);

  param.ResetValue();
  EXPECT_FLOAT_EQ(param.GetValue(), 10.0f);
}

}  // namespace
}  // namespace barelyapi
