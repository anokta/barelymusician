#include "barelymusician/instrument/modulation_matrix.h"

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Test parameter properties.
const int kParamId = 1;
const float kDefaultParamValue = 1.0f;
const float kParamValue = 0.5f;

// Tests that invalid parameter operations fail as expected.
TEST(ModulationMatrixTest, InvalidParamFails) {
  const int kInvalidParamId = -1;

  ModulationMatrix<float> modulation_matrix;
  EXPECT_FALSE(modulation_matrix.SetParam(kInvalidParamId, kDefaultParamValue));
  EXPECT_FALSE(modulation_matrix.GetParam(kInvalidParamId, nullptr));
}

// Tests that registering a parameter sets its default value as expected.
TEST(ModulationMatrixTest, DefaultParam) {
  ModulationMatrix<float> modulation_matrix;
  modulation_matrix.Register(kParamId, kDefaultParamValue);

  float value = 0.0f;
  EXPECT_TRUE(modulation_matrix.GetParam(kParamId, &value));
  EXPECT_FLOAT_EQ(value, kDefaultParamValue);
}

// Tests that setting the value of a parameter can be accessed as expected.
TEST(ModulationMatrixTest, SetParam) {
  ModulationMatrix<float> modulation_matrix;
  modulation_matrix.Register(kParamId, kDefaultParamValue);

  EXPECT_TRUE(modulation_matrix.SetParam(kParamId, kParamValue));

  float value = 0.0f;
  EXPECT_TRUE(modulation_matrix.GetParam(kParamId, &value));
  EXPECT_FLOAT_EQ(value, kParamValue);
}

// Tests that the parameter update function gets called accordingly with the
// expected values.
TEST(ModulationMatrixTest, UpdateParam) {
  ModulationMatrix<float> modulation_matrix;

  float param_value = 0.0f;
  const auto update_param_fn = [&param_value](float value) {
    param_value = value;
  };
  modulation_matrix.Register(kParamId, kDefaultParamValue, update_param_fn);
  EXPECT_FLOAT_EQ(param_value, kDefaultParamValue);

  EXPECT_TRUE(modulation_matrix.SetParam(kParamId, kParamValue));
  EXPECT_FLOAT_EQ(param_value, kParamValue);
}

}  // namespace
}  // namespace barelyapi
