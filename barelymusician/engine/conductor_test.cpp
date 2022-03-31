#include "barelymusician/engine/conductor.h"

#include <cmath>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that conductor returns notes as expected.
TEST(ConductorTest, GetNote) {
  const double kPitch = 0.25;
  const double kRootPitch = -1.75;
  const std::vector<double> kScale = {0.0, 0.2, 0.6, 0.75};

  Conductor conductor;
  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::AbsolutePitch(kPitch)), kPitch);
  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::RelativePitch(kPitch)), kPitch);
  for (int i = 0; i < static_cast<int>(kScale.size()); ++i) {
    EXPECT_DOUBLE_EQ(conductor.GetNote(Note::PitchDefinition::ScaleIndex(i)),
                     0.0);
    EXPECT_DOUBLE_EQ(conductor.GetNote(Note::PitchDefinition::ScaleIndex(-i)),
                     0.0);
  }

  // Set root note.
  conductor.SetRootNote(kRootPitch);
  EXPECT_DOUBLE_EQ(conductor.GetRootNote(), kRootPitch);

  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::AbsolutePitch(kPitch)), kPitch);
  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::RelativePitch(kPitch)),
      kRootPitch + kPitch);
  for (int i = 0; i < static_cast<int>(kScale.size()); ++i) {
    EXPECT_DOUBLE_EQ(conductor.GetNote(Note::PitchDefinition::ScaleIndex(i)),
                     kRootPitch);
    EXPECT_DOUBLE_EQ(conductor.GetNote(Note::PitchDefinition::ScaleIndex(-i)),
                     kRootPitch);
  }

  // Set scale.
  conductor.SetScale(kScale);
  EXPECT_EQ(conductor.GetScale(), kScale);

  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::AbsolutePitch(kPitch)), kPitch);
  EXPECT_DOUBLE_EQ(
      conductor.GetNote(Note::PitchDefinition::RelativePitch(kPitch)),
      kRootPitch + kPitch);
  for (int i = 0; i < static_cast<int>(kScale.size()); ++i) {
    EXPECT_DOUBLE_EQ(conductor.GetNote(Note::PitchDefinition::ScaleIndex(i)),
                     kRootPitch + kScale[i]);
    EXPECT_DOUBLE_EQ(
        conductor.GetNote(Note::PitchDefinition::ScaleIndex(-i)),
        kRootPitch - (i > 0 ? 1.0 - kScale[kScale.size() - i] : 0.0));
  }
}

// Tests that conductor transforms note definition as expected.
TEST(ConductorTest, TransformNoteDefinition) {
  const Note kNote = {4.0, -0.5, 1.0};
  const Note::Definition kDefinition = Note::Definition(
      kNote.duration, Note::PitchDefinition::AbsolutePitch(kNote.pitch),
      kNote.intensity);

  Conductor conductor;
  EXPECT_EQ(conductor.TransformNote(kDefinition, false), kNote);
  EXPECT_EQ(conductor.TransformNote(kDefinition, true), kNote);

  conductor.SetAdjustNoteDefinitionCallback([](Note::Definition* definition) {
    definition->duration += 0.5;
    definition->pitch.absolute_pitch *= 2.0;
    definition->intensity = 0.1;
  });
  EXPECT_EQ(conductor.TransformNote(kDefinition, false),
            Note(kNote.duration + 0.5, kNote.pitch * 2.0, 0.1));
  EXPECT_EQ(conductor.TransformNote(kDefinition, true), kNote);
}

}  // namespace
}  // namespace barelyapi
