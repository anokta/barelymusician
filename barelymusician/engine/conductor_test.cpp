#include "barelymusician/engine/conductor.h"

#include <variant>

#include "barelymusician/common/status.h"
#include "barelymusician/composition/note_duration.h"
#include "barelymusician/composition/note_intensity.h"
#include "barelymusician/composition/note_pitch.h"
#include "barelymusician/engine/conductor_definition.h"
#include "barelymusician/engine/param_definition.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Returns test conductor definition.
ConductorDefinition GetTestConductorDefinition() {
  return ConductorDefinition{
      .create_fn =
          [](void** state) {
            *state = reinterpret_cast<void*>(new float{0.0f});
          },
      .destroy_fn =
          [](void** state) { delete reinterpret_cast<float*>(*state); },
      .set_param_fn =
          [](void** state, int index, float value) {
            *reinterpret_cast<float*>(*state) =
                static_cast<float>(index + 1) * value;
          },
      .transform_note_duration_fn =
          [](void** state, const NoteDuration& note_duration) {
            return static_cast<double>(*reinterpret_cast<float*>(*state)) *
                   std::get<double>(note_duration);
          },
      .transform_note_intensity_fn =
          [](void** state, const NoteIntensity& note_intensity) {
            return *reinterpret_cast<float*>(*state) *
                   std::get<float>(note_intensity);
          },
      .transform_note_pitch_fn =
          [](void** state, const NotePitch& note_pitch) {
            return std::get<float>(note_pitch) -
                   *reinterpret_cast<float*>(*state);
          },
      .transform_playback_tempo_fn =
          [](void** state, double tempo) {
            return tempo +
                   static_cast<double>(*reinterpret_cast<float*>(*state));
          },
      .param_definitions = {ParamDefinition{0.0f}}};
}

// Tests that the conductor behaves as expected with an empty definition.
TEST(ConductorTest, EmptyDefinition) {
  Conductor conductor;
  EXPECT_DOUBLE_EQ(
      GetStatusOrValue(conductor.TransformNoteDuration(NoteDuration{5.0})),
      5.0);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNoteIntensity(NoteIntensity{1.0f})),
      1.0f);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNotePitch(NotePitch{-0.4f})), -0.4f);
  EXPECT_DOUBLE_EQ(conductor.TransformPlaybackTempo(100.0), 100.0);
}

// Tests that the conductor behaves as expected with a test definition.
TEST(ConductorTest, TestDefinition) {
  Conductor conductor(GetTestConductorDefinition());
  EXPECT_DOUBLE_EQ(
      GetStatusOrValue(conductor.TransformNoteDuration(NoteDuration{5.0})),
      0.0);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNoteIntensity(NoteIntensity{1.0f})),
      0.0f);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNotePitch(NotePitch{-0.4f})), -0.4f);
  EXPECT_DOUBLE_EQ(conductor.TransformPlaybackTempo(100.0), 100.0);

  conductor.SetParam(0, 10.0f);
  EXPECT_DOUBLE_EQ(
      GetStatusOrValue(conductor.TransformNoteDuration(NoteDuration{5.0})),
      50.0);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNoteIntensity(NoteIntensity{1.0f})),
      10.0f);
  EXPECT_FLOAT_EQ(
      GetStatusOrValue(conductor.TransformNotePitch(NotePitch{-0.4f})), -10.4f);
  EXPECT_DOUBLE_EQ(conductor.TransformPlaybackTempo(100.0), 110.0);
}

}  // namespace
}  // namespace barelyapi
