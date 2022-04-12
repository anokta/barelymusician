#include "barelymusician/engine/sequence.h"

#include <vector>

#include "barelymusician/engine/conductor.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "barelymusician/engine/transport.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Pointee;

// Tests that sequence processes a single note as expected.
TEST(SequenceTest, ProcessSingleNote) {
  const Id kId = 1;
  const Note::Definition kDefinition = {
      1.0, Note::PitchDefinition::AbsolutePitch(10.0)};
  const double kPosition = 5.0;

  Conductor conductor;
  Transport transport;
  transport.SetTempo(60.0);

  Sequence sequence(conductor, transport);
  EXPECT_THAT(sequence.GetInstrument(), IsNull());
  EXPECT_THAT(sequence.GetNoteDefinition(kId), IsNull());
  EXPECT_THAT(sequence.GetNotePosition(kId), IsNull());

  // Set instrument.
  std::vector<double> note_on_timestamps;
  std::vector<double> note_off_timestamps;
  Instrument instrument(Instrument::Definition({}), 1);
  instrument.SetNoteOnCallback(
      [&](double pitch, double intensity, double timestamp) {
        EXPECT_DOUBLE_EQ(pitch, kDefinition.pitch.absolute_pitch);
        EXPECT_DOUBLE_EQ(intensity, kDefinition.intensity);
        note_on_timestamps.push_back(timestamp);
      });
  instrument.SetNoteOffCallback([&](double pitch, double timestamp) {
    EXPECT_DOUBLE_EQ(pitch, kDefinition.pitch.absolute_pitch);
    note_off_timestamps.push_back(timestamp);
  });

  sequence.SetInstrument(&instrument);
  EXPECT_THAT(sequence.GetInstrument(), &instrument);

  // Create note.
  EXPECT_TRUE(sequence.CreateNote(kId, kDefinition, kPosition));
  EXPECT_THAT(
      sequence.GetNoteDefinition(kId),
      AllOf(NotNull(),
            Pointee(AllOf(
                Field(&Note::Definition::duration, 1.0),
                Field(&Note::Definition::pitch,
                      Field(&Note::PitchDefinition::absolute_pitch, 10.0)),
                Field(&Note::Definition::intensity, 1.0)))));
  EXPECT_THAT(sequence.GetNotePosition(kId),
              AllOf(NotNull(), Pointee(kPosition)));

  // Process before the note position.
  sequence.Process(0.0, 1.0);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process just before the note position.
  sequence.Process(4.0, 5.0);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process starting with the note position.
  sequence.Process(5.0, 6.0);
  EXPECT_THAT(note_on_timestamps, ElementsAre(5.0));
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process overlapping the note position.
  sequence.Process(4.75, 5.5);
  EXPECT_THAT(note_on_timestamps, ElementsAre(5.0));
  EXPECT_THAT(note_off_timestamps, ElementsAre(4.75));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process just after the note position.
  sequence.Process(6.0, 7.0);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_THAT(note_off_timestamps, ElementsAre(6.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process after the note position, but with a begin position to compensate.
  sequence.SetBeginPosition(1.5);
  EXPECT_DOUBLE_EQ(sequence.GetBeginPosition(), 1.5);

  sequence.Process(6.0, 7.0);
  EXPECT_THAT(note_on_timestamps, ElementsAre(6.5));
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Set begin offset.
  sequence.SetBeginOffset(4.0);
  EXPECT_DOUBLE_EQ(sequence.GetBeginOffset(), 4.0);

  sequence.Process(2.0, 3.0);
  EXPECT_THAT(note_on_timestamps, ElementsAre(2.5));
  EXPECT_THAT(note_off_timestamps, ElementsAre(2.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Set looping.
  sequence.SetLooping(true);
  EXPECT_TRUE(sequence.IsLooping());

  sequence.Process(1.0, 11.0);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_THAT(note_off_timestamps, ElementsAre(1.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Set loop begin offset and length with an end position.
  sequence.SetLoopBeginOffset(2.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopBeginOffset(), 2.0);
  sequence.SetLoopLength(4.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopLength(), 4.0);
  sequence.SetEndPosition(11.0);
  EXPECT_DOUBLE_EQ(sequence.GetEndPosition(), 11.0);

  sequence.Process(1.5, 11.5);
  EXPECT_THAT(note_on_timestamps, ElementsAre(2.5, 6.5, 10.5));
  EXPECT_THAT(note_off_timestamps, ElementsAre(3.5, 7.5, 11.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Destroy note.
  EXPECT_TRUE(sequence.DestroyNote(kId));
  EXPECT_THAT(sequence.GetNoteDefinition(kId), IsNull());
  EXPECT_THAT(sequence.GetNotePosition(kId), IsNull());

  sequence.Process(1.0, 11.0);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
}

// Tests that sequence processes multiple notes as expected.
TEST(SequenceTest, ProcessMultipleNotes) {
  Conductor conductor;
  Transport transport;
  transport.SetTempo(60.0);

  Sequence sequence(conductor, transport);
  EXPECT_THAT(sequence.GetInstrument(), IsNull());

  // Set instrument.
  std::vector<std::pair<double, double>> note_ons;
  std::vector<std::pair<double, double>> note_offs;
  Instrument instrument(Instrument::Definition({}), 1);
  instrument.SetNoteOnCallback(
      [&](double pitch, double intensity, double timestamp) {
        EXPECT_DOUBLE_EQ(intensity, 1.0);
        note_ons.emplace_back(pitch, timestamp);
      });
  instrument.SetNoteOffCallback([&](double pitch, double timestamp) {
    note_offs.emplace_back(pitch, timestamp);
  });

  sequence.SetInstrument(&instrument);
  EXPECT_THAT(sequence.GetInstrument(), &instrument);

  // Create notes.
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(sequence.CreateNote(
        static_cast<Id>(i + 1),
        Note::Definition(1.0, Note::PitchDefinition::AbsolutePitch(
                                  static_cast<double>(i + 1))),
        static_cast<double>(i)));
    EXPECT_THAT(sequence.GetNotePosition(static_cast<Id>(i + 1)),
                AllOf(NotNull(), Pointee(static_cast<double>(i))));
  }

  // Process until the end of last note.
  sequence.Process(0.0, 4.0);
  EXPECT_THAT(note_ons, ElementsAre(Pair(1.0, 0.0), Pair(2.0, 1.0),
                                    Pair(3.0, 2.0), Pair(4.0, 3.0)));
  EXPECT_THAT(note_offs,
              ElementsAre(Pair(1.0, 1.0), Pair(2.0, 2.0), Pair(3.0, 3.0)));
  note_ons.clear();
  note_offs.clear();

  // Stop the last note.
  sequence.Stop();
  EXPECT_TRUE(note_ons.empty());
  EXPECT_THAT(note_offs, ElementsAre(Pair(4.0, 0.0)));
  note_ons.clear();
  note_offs.clear();

  // Set loop with offset.
  sequence.SetBeginOffset(1.0);
  EXPECT_DOUBLE_EQ(sequence.GetBeginOffset(), 1.0);
  sequence.SetLooping(true);
  EXPECT_TRUE(sequence.IsLooping());
  sequence.SetLoopBeginOffset(2.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopBeginOffset(), 2.0);
  sequence.SetLoopLength(2.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopLength(), 2.0);

  // Set end position.
  sequence.SetEndPosition(9.5);
  EXPECT_DOUBLE_EQ(sequence.GetEndPosition(), 9.5);

  sequence.Process(0.0, 10.0);
  EXPECT_THAT(note_ons,
              ElementsAre(Pair(2.0, 0.0), Pair(3.0, 1.0), Pair(4.0, 2.0),
                          Pair(3.0, 3.0), Pair(4.0, 4.0), Pair(3.0, 5.0),
                          Pair(4.0, 6.0), Pair(3.0, 7.0), Pair(4.0, 8.0),
                          Pair(3.0, 9.0)));
  EXPECT_THAT(note_offs,
              ElementsAre(Pair(2.0, 1.0), Pair(3.0, 2.0), Pair(4.0, 3.0),
                          Pair(3.0, 4.0), Pair(4.0, 5.0), Pair(3.0, 6.0),
                          Pair(4.0, 7.0), Pair(3.0, 8.0), Pair(4.0, 9.0),
                          Pair(3.0, 9.5)));
  note_ons.clear();
  note_offs.clear();

  // Destroy all notes.
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(sequence.DestroyNote(static_cast<Id>(i + 1)));
  }

  sequence.Process(0.0, 10.0);
  EXPECT_TRUE(note_ons.empty());
  EXPECT_TRUE(note_offs.empty());
}

}  // namespace
}  // namespace barelyapi
