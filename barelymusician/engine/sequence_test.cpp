#include "barelymusician/engine/sequence.h"

#include <vector>

#include "barelymusician/engine/clock.h"
#include "barelymusician/engine/id.h"
#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/note.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barely::internal {
namespace {

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Pointee;

// Tests that sequence processes a single note as expected.
TEST(SequenceTest, ProcessSingleNote) {
  const Id kId = 1;
  const Note kNote = {1.0, 10.0, 0.25};
  const double kPosition = 5.0;

  Clock clock;
  Sequence sequence(clock);
  EXPECT_THAT(sequence.GetInstrument(), IsNull());
  EXPECT_THAT(sequence.GetNote(kId), IsNull());
  EXPECT_THAT(sequence.GetNotePosition(kId), IsNull());

  // Set instrument.
  std::vector<double> note_on_timestamps;
  std::vector<double> note_off_timestamps;
  Instrument instrument(Instrument::Definition({}), 1);
  instrument.SetNoteOnCallback(
      [&](double pitch, double intensity, double timestamp) {
        EXPECT_DOUBLE_EQ(pitch, kNote.pitch);
        EXPECT_DOUBLE_EQ(intensity, kNote.intensity);
        note_on_timestamps.push_back(timestamp);
      });
  instrument.SetNoteOffCallback([&](double pitch, double timestamp) {
    EXPECT_DOUBLE_EQ(pitch, kNote.pitch);
    note_off_timestamps.push_back(timestamp);
  });

  sequence.SetInstrument(&instrument);
  EXPECT_THAT(sequence.GetInstrument(), &instrument);

  // Create note.
  EXPECT_TRUE(sequence.CreateNote(kId, kPosition, kNote));
  EXPECT_THAT(sequence.GetNote(kId), AllOf(NotNull(), Pointee(kNote)));
  EXPECT_THAT(sequence.GetNotePosition(kId),
              AllOf(NotNull(), Pointee(kPosition)));

  const auto get_timestamp_fn = [](double position) { return position; };

  // Process before the note position.
  sequence.Process(0.0, 1.0, get_timestamp_fn);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process just before the note position.
  sequence.Process(4.0, 5.0, get_timestamp_fn);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process starting with the note position.
  sequence.Process(5.0, 6.0, get_timestamp_fn);
  EXPECT_THAT(note_on_timestamps, ElementsAre(5.0));
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process overlapping the note position.
  sequence.Process(4.75, 5.5, get_timestamp_fn);
  EXPECT_THAT(note_on_timestamps, ElementsAre(5.0));
  EXPECT_THAT(note_off_timestamps, ElementsAre(4.75));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process just after the note position.
  sequence.Process(6.0, 7.0, get_timestamp_fn);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_THAT(note_off_timestamps, ElementsAre(6.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Process after the note position, but with a begin position to compensate.
  sequence.SetBeginPosition(1.5);
  EXPECT_DOUBLE_EQ(sequence.GetBeginPosition(), 1.5);

  sequence.Process(6.0, 7.0, get_timestamp_fn);
  EXPECT_THAT(note_on_timestamps, ElementsAre(6.5));
  EXPECT_TRUE(note_off_timestamps.empty());
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Set begin offset.
  sequence.SetBeginOffset(4.0);
  EXPECT_DOUBLE_EQ(sequence.GetBeginOffset(), 4.0);

  sequence.Process(2.0, 3.0, get_timestamp_fn);
  EXPECT_THAT(note_on_timestamps, ElementsAre(2.5));
  EXPECT_THAT(note_off_timestamps, ElementsAre(2.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Set looping.
  sequence.SetLooping(true);
  EXPECT_TRUE(sequence.IsLooping());

  sequence.Process(1.0, 11.0, get_timestamp_fn);
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

  sequence.Process(1.5, 11.5, get_timestamp_fn);
  EXPECT_THAT(note_on_timestamps, ElementsAre(2.5, 6.5, 10.5));
  EXPECT_THAT(note_off_timestamps, ElementsAre(3.5, 7.5, 11.0));
  note_on_timestamps.clear();
  note_off_timestamps.clear();

  // Destroy note.
  EXPECT_TRUE(sequence.DestroyNote(kId));
  EXPECT_THAT(sequence.GetNote(kId), IsNull());
  EXPECT_THAT(sequence.GetNotePosition(kId), IsNull());

  sequence.Process(1.0, 11.0, get_timestamp_fn);
  EXPECT_TRUE(note_on_timestamps.empty());
  EXPECT_TRUE(note_off_timestamps.empty());
}

// Tests that sequence processes multiple notes as expected.
TEST(SequenceTest, ProcessMultipleNotes) {
  Clock clock;
  Sequence sequence(clock);
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
    const Id note_id = i + 1;
    const Note note = {1.0, static_cast<double>(i + 1), 1.0};
    EXPECT_TRUE(sequence.CreateNote(note_id, static_cast<double>(i), note));
    EXPECT_THAT(sequence.GetNote(note_id), AllOf(NotNull(), Pointee(note)));
    EXPECT_THAT(sequence.GetNotePosition(note_id),
                AllOf(NotNull(), Pointee(static_cast<double>(i))));
  }

  const auto get_timestamp_fn = [](double position) { return position; };

  // Process until the end of last note.
  sequence.Process(0.0, 4.0, get_timestamp_fn);
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

  sequence.Process(0.0, 10.0, get_timestamp_fn);
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

  sequence.Process(0.0, 10.0, get_timestamp_fn);
  EXPECT_TRUE(note_ons.empty());
  EXPECT_TRUE(note_offs.empty());
}

}  // namespace
}  // namespace barely::internal
