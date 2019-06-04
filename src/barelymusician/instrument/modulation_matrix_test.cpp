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

// Tests that the parameter updater gets called accordingly with the expected
// values.
TEST(ModulationMatrixTest, ParamUpdater) {
  ModulationMatrix<float> modulation_matrix;

  float update_value = 0.0f;
  const auto param_updater = [&update_value](float value) {
    update_value = value;
  };
  modulation_matrix.Register(kParamId, kDefaultParamValue, param_updater);
  EXPECT_FLOAT_EQ(update_value, kDefaultParamValue);

  EXPECT_TRUE(modulation_matrix.SetParam(kParamId, kParamValue));
  EXPECT_FLOAT_EQ(update_value, kParamValue);

  modulation_matrix.Reset();
  EXPECT_FLOAT_EQ(update_value, kDefaultParamValue);
}

// Tests that resetting reverts the parameter to its default value.
TEST(ModulationMatrixTest, Reset) {
  ModulationMatrix<float> modulation_matrix;
  modulation_matrix.Register(kParamId, kDefaultParamValue);

  EXPECT_TRUE(modulation_matrix.SetParam(kParamId, kParamValue));

  modulation_matrix.Reset();

  float value = 0.0f;
  EXPECT_TRUE(modulation_matrix.GetParam(kParamId, &value));
  EXPECT_FLOAT_EQ(value, kDefaultParamValue);
}

}  // namespace
}  // namespace barelyapi
