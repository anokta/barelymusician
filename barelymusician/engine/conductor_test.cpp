#include "barelymusician/engine/conductor.h"

#include <variant>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/parameter.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Returns test conductor definition.
BarelyConductorDefinition GetTestConductorDefinition() {
  static std::vector<BarelyParameterDefinition> parameter_definitions = {
      BarelyParameterDefinition{0.0f, 0.0f, 100.0f}};
  return BarelyConductorDefinition{
      .adjust_note_duration_fn =
          [](void** state, double* duration) {
            *duration *= static_cast<double>(*reinterpret_cast<float*>(*state));
          },
      .adjust_note_intensity_fn =
          [](void** state, float* intensity) {
            *intensity *= *reinterpret_cast<float*>(*state);
          },
      .adjust_note_pitch_fn =
          [](void** state, BarelyNotePitchType* /*pitch_type*/, float* pitch) {
            *pitch -= *reinterpret_cast<float*>(*state);
          },
      .adjust_tempo_fn =
          [](void** state, double* tempo) {
            *tempo += static_cast<double>(*reinterpret_cast<float*>(*state));
          },
      .create_fn =
          [](void** state) {
            *state = reinterpret_cast<void*>(new float{0.0f});
          },
      .destroy_fn =
          [](void** state) { delete reinterpret_cast<float*>(*state); },
      .set_parameter_fn =
          [](void** state, int index, float value) {
            *reinterpret_cast<float*>(*state) =
                static_cast<float>(index + 1) * value;
          },
      .parameter_definitions = parameter_definitions.data(),
      .num_parameter_definitions =
          static_cast<int>(parameter_definitions.size())};
}

// Tests that the conductor behaves as expected with an empty definition.
TEST(ConductorTest, EmptyDefinition) {
  Conductor conductor;
  EXPECT_DOUBLE_EQ(conductor.AdjustNoteDuration(NoteDuration{5.0}), 5.0);
  EXPECT_FLOAT_EQ(conductor.AdjustNoteIntensity(NoteIntensity{1.0f}), 1.0f);
  EXPECT_FLOAT_EQ(conductor.AdjustNotePitch(NotePitch{-0.4f}), -0.4f);
  EXPECT_DOUBLE_EQ(conductor.AdjustTempo(100.0), 100.0);
}

// Tests that the conductor behaves as expected with a test definition.
TEST(ConductorTest, TestDefinition) {
  Conductor conductor(GetTestConductorDefinition());
  EXPECT_DOUBLE_EQ(conductor.AdjustNoteDuration(NoteDuration{5.0}), 0.0);
  EXPECT_FLOAT_EQ(conductor.AdjustNoteIntensity(NoteIntensity{1.0f}), 0.0f);
  EXPECT_FLOAT_EQ(conductor.AdjustNotePitch(NotePitch{-0.4f}), -0.4f);
  EXPECT_DOUBLE_EQ(conductor.AdjustTempo(100.0), 100.0);

  conductor.SetParameter(0, 10.0f);
  EXPECT_DOUBLE_EQ(conductor.AdjustNoteDuration(NoteDuration{5.0}), 50.0);
  EXPECT_FLOAT_EQ(conductor.AdjustNoteIntensity(NoteIntensity{1.0f}), 10.0f);
  EXPECT_FLOAT_EQ(conductor.AdjustNotePitch(NotePitch{-0.4f}), -10.4f);
  EXPECT_DOUBLE_EQ(conductor.AdjustTempo(100.0), 110.0);
}

}  // namespace
}  // namespace barelyapi
