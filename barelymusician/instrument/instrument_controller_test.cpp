#include "barelymusician/instrument/instrument_controller.h"

#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

// Returns test instrument parameter definition.
InstrumentParamDefinitions GetTestInstrumentParamDefinitions() {
  return InstrumentParamDefinitions{
      InstrumentParamDefinition{0, 0.0f},
      InstrumentParamDefinition{1, 1.0f, 0.0f},
      InstrumentParamDefinition{10, 10.0f, 10.0f, 20.0f}};
}

// Tests that controller resets parameter values as expected.
TEST(InstrumentControllerTest, ResetAllParams) {
  InstrumentController controller(GetTestInstrumentParamDefinitions());
  EXPECT_THAT(
      controller.GetAllParams(),
      UnorderedElementsAre(Pair(0, 0.0f), Pair(1, 1.0f), Pair(10, 10.0f)));

  EXPECT_TRUE(controller.SetParam(0, -1.0f));
  EXPECT_TRUE(controller.SetParam(1, 5.0f));
  EXPECT_TRUE(controller.SetParam(10, 15.0f));
  EXPECT_THAT(
      controller.GetAllParams(),
      UnorderedElementsAre(Pair(0, -1.0f), Pair(1, 5.0f), Pair(10, 15.0f)));

  controller.ResetAllParams();
  EXPECT_THAT(
      controller.GetAllParams(),
      UnorderedElementsAre(Pair(0, 0.0f), Pair(1, 1.0f), Pair(10, 10.0f)));
}

// Tests that controller sets all notes off as expected.
TEST(InstrumentControllerTest, SetAllNotesOff) {
  InstrumentController controller(GetTestInstrumentParamDefinitions());
  EXPECT_TRUE(controller.GetAllNotes().empty());

  EXPECT_TRUE(controller.SetNoteOn(1.0f));
  EXPECT_TRUE(controller.SetNoteOn(2.0f));
  EXPECT_TRUE(controller.SetNoteOn(5.0f));
  EXPECT_THAT(controller.GetAllNotes(), UnorderedElementsAre(1.0f, 2.0f, 5.0f));

  controller.SetAllNotesOff();
  EXPECT_TRUE(controller.GetAllNotes().empty());
}

// Tests that controller sets and returns notes as expected.
TEST(InstrumentControllerTest, SetNote) {
  InstrumentController controller(GetTestInstrumentParamDefinitions());

  EXPECT_FALSE(controller.IsNoteOn(10.0f));

  EXPECT_TRUE(controller.SetNoteOn(10.0f));
  EXPECT_TRUE(controller.IsNoteOn(10.0f));
  EXPECT_FALSE(controller.SetNoteOn(10.0f));

  EXPECT_TRUE(controller.IsNoteOn(10.0f));

  EXPECT_TRUE(controller.SetNoteOff(10.0f));
  EXPECT_FALSE(controller.IsNoteOn(10.0f));
  EXPECT_FALSE(controller.SetNoteOff(10.0f));

  EXPECT_FALSE(controller.IsNoteOn(10.0f));
}

// Tests that controller sets and returns parameter values as expected.
TEST(InstrumentControllerTest, SetParam) {
  InstrumentController controller(GetTestInstrumentParamDefinitions());

  EXPECT_FLOAT_EQ(*controller.GetParam(0), 0.0f);
  EXPECT_TRUE(controller.SetParam(0, 5.0f));
  EXPECT_FLOAT_EQ(*controller.GetParam(0), 5.0f);
  EXPECT_TRUE(controller.ResetParam(0));
  EXPECT_FLOAT_EQ(*controller.GetParam(0), 0.0f);

  EXPECT_FLOAT_EQ(*controller.GetParam(1), 1.0f);
  EXPECT_TRUE(controller.SetParam(1, 10.0f));
  EXPECT_FLOAT_EQ(*controller.GetParam(1), 10.0f);
  // Should be clamped to the parameter minimum value.
  EXPECT_TRUE(controller.SetParam(1, -10.0f));
  EXPECT_FLOAT_EQ(*controller.GetParam(1), 0.0f);
  EXPECT_TRUE(controller.ResetParam(1));
  EXPECT_FLOAT_EQ(*controller.GetParam(1), 1.0f);

  EXPECT_FLOAT_EQ(*controller.GetParam(10), 10.0f);
  EXPECT_TRUE(controller.SetParam(10, 15.0f));
  EXPECT_FLOAT_EQ(*controller.GetParam(10), 15.0f);
  // Should be clamped to the parameter maximum value.
  EXPECT_TRUE(controller.SetParam(10, 100.0f));
  EXPECT_FLOAT_EQ(*controller.GetParam(10), 20.0f);
  EXPECT_TRUE(controller.ResetParam(10));
  EXPECT_FLOAT_EQ(*controller.GetParam(10), 10.0f);

  EXPECT_FALSE(controller.SetParam(2, 2.0f));
  EXPECT_FALSE(controller.ResetParam(2));
}

}  // namespace
}  // namespace barelyapi
