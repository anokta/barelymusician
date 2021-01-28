#include "barelymusician/instrument/instrument_controller.h"

#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::Pair;
using ::testing::UnorderedElementsAre;
using ::testing::VariantWith;

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
      UnorderedElementsAre(Param{0, 0.0f}, Param{1, 1.0f}, Param{10, 10.0f}));

  EXPECT_TRUE(controller.SetParam(0, -1.0f));
  EXPECT_TRUE(controller.SetParam(1, 5.0f));
  EXPECT_TRUE(controller.SetParam(10, 15.0f));
  EXPECT_THAT(
      controller.GetAllParams(),
      UnorderedElementsAre(Param{0, -1.0f}, Param{1, 5.0f}, Param{10, 15.0f}));

  controller.ResetAllParams();
  EXPECT_THAT(
      controller.GetAllParams(),
      UnorderedElementsAre(Param{0, 0.0f}, Param{1, 1.0f}, Param{10, 10.0f}));
}

// Tests that the controller schedules a single note as expected.
TEST(InstrumentControllerTest, ScheduleSingleNote) {
  const double kPosition = 4.0f;
  const double kDuration = 0.5f;
  const float kPitch = 2.5f;
  const float kIntensity = 0.25f;

  InstrumentController controller(GetTestInstrumentParamDefinitions());
  EXPECT_TRUE(controller.GetAllScheduledData().empty());

  // Add note.
  controller.ScheduleNote(kPosition, kDuration, kPitch, kIntensity);
  EXPECT_THAT(
      controller.GetAllScheduledData(),
      UnorderedElementsAre(
          Pair(kPosition, VariantWith<NoteOn>(NoteOn{kPitch, kIntensity})),
          Pair(kPosition + kDuration, VariantWith<NoteOff>(NoteOff{kPitch}))));
  EXPECT_TRUE(controller.GetAllScheduledData(0.0, kPosition).empty());
  EXPECT_THAT(controller.GetAllScheduledData(kPosition, kPosition + kDuration),
              UnorderedElementsAre(Pair(
                  kPosition, VariantWith<NoteOn>(NoteOn{kPitch, kIntensity}))));

  // Remove note.
  controller.RemoveAllScheduledData();
  EXPECT_TRUE(controller.GetAllScheduledData().empty());
}

// Tests that the controller schedules multiple notes as expected.
TEST(TrackTest, MultipleNotes) {
  const float kIntensity = 0.25f;

  InstrumentController controller(GetTestInstrumentParamDefinitions());
  EXPECT_TRUE(controller.GetAllScheduledData().empty());

  // Add notes.
  for (int i = 0; i < 5; ++i) {
    controller.ScheduleNote(static_cast<double>(i), 1.0, static_cast<float>(i),
                            kIntensity);
  }
  EXPECT_THAT(controller.GetAllScheduledData(),
              UnorderedElementsAre(
                  Pair(0.0, VariantWith<NoteOn>(NoteOn{0.0f, kIntensity})),
                  Pair(1.0, VariantWith<NoteOff>(NoteOff{0.0f})),
                  Pair(1.0, VariantWith<NoteOn>(NoteOn{1.0f, kIntensity})),
                  Pair(2.0, VariantWith<NoteOff>(NoteOff{1.0f})),
                  Pair(2.0, VariantWith<NoteOn>(NoteOn{2.0f, kIntensity})),
                  Pair(3.0, VariantWith<NoteOff>(NoteOff{2.0f})),
                  Pair(3.0, VariantWith<NoteOn>(NoteOn{3.0f, kIntensity})),
                  Pair(4.0, VariantWith<NoteOff>(NoteOff{3.0f})),
                  Pair(4.0, VariantWith<NoteOn>(NoteOn{4.0f, kIntensity})),
                  Pair(5.0, VariantWith<NoteOff>(NoteOff{4.0f}))));
  EXPECT_THAT(controller.GetAllScheduledData(0.0, 5.0),
              UnorderedElementsAre(
                  Pair(0.0, VariantWith<NoteOn>(NoteOn{0.0f, kIntensity})),
                  Pair(1.0, VariantWith<NoteOff>(NoteOff{0.0f})),
                  Pair(1.0, VariantWith<NoteOn>(NoteOn{1.0f, kIntensity})),
                  Pair(2.0, VariantWith<NoteOff>(NoteOff{1.0f})),
                  Pair(2.0, VariantWith<NoteOn>(NoteOn{2.0f, kIntensity})),
                  Pair(3.0, VariantWith<NoteOff>(NoteOff{2.0f})),
                  Pair(3.0, VariantWith<NoteOn>(NoteOn{3.0f, kIntensity})),
                  Pair(4.0, VariantWith<NoteOff>(NoteOff{3.0f})),
                  Pair(4.0, VariantWith<NoteOn>(NoteOn{4.0f, kIntensity}))));
  EXPECT_THAT(
      controller.GetAllScheduledData(5.0, 6.0),
      UnorderedElementsAre(Pair(5.0, VariantWith<NoteOff>(NoteOff{4.0f}))));
  EXPECT_TRUE(controller.GetAllScheduledData(6.0, 7.0).empty());

  // Remove notes.
  controller.RemoveAllScheduledData();
  EXPECT_TRUE(controller.GetAllScheduledData().empty());
  EXPECT_TRUE(controller.GetAllScheduledData(0.0, 6.0).empty());
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
