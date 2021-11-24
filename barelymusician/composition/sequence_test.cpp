#include "barelymusician/composition/sequence.h"

#include "barelymusician/common/id.h"
#include "barelymusician/common/status.h"
#include "barelymusician/composition/note.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::ElementsAre;
using ::testing::Pair;

// Tests that a single sequence note is processed as expected.
TEST(SequenceTest, ProcessSingleNote) {
  const Id kId = 1;
  const Note kNote = Note{.pitch = 10.0f, .duration = 1.0};

  Sequence sequence;
  EXPECT_TRUE(sequence.GetAllNotes().empty());
  EXPECT_DOUBLE_EQ(sequence.GetBeginOffset(), 0.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopBeginOffset(), 0.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopLength(), 1.0);
  EXPECT_THAT(GetStatusOrStatus(sequence.GetNote(kId)), Status::kNotFound);
  EXPECT_TRUE(sequence.IsEmpty());
  EXPECT_FALSE(sequence.IsLooping());

  // Add note.
  EXPECT_TRUE(IsOk(sequence.AddNote(kId, 5.0, kNote)));
  EXPECT_THAT(GetStatusOrValue(sequence.GetNote(kId)), Pair(5.0, kNote));
  EXPECT_FALSE(sequence.IsEmpty());

  std::vector<Sequence::NoteWithPosition> notes;
  const auto process_callback = [&](double position, const Note& note) {
    notes.emplace_back(position, note);
  };

  // Process before the note position.
  sequence.Process(0.0, 1.0, 0.0, process_callback);
  EXPECT_TRUE(notes.empty());
  notes.clear();

  // Process just before the note position.
  sequence.Process(4.0, 5.0, 0.0, process_callback);
  EXPECT_TRUE(notes.empty());
  notes.clear();

  // Process starting with the note position.
  sequence.Process(5.0, 6.0, 0.0, process_callback);
  EXPECT_THAT(notes, ElementsAre(Pair(5.0, kNote)));
  notes.clear();

  // Process overlapping the note position.
  sequence.Process(4.75, 5.5, 0.0, process_callback);
  EXPECT_THAT(notes, ElementsAre(Pair(5.0, kNote)));
  notes.clear();

  // Process just after the note position.
  sequence.Process(6.0, 7.0, 0.0, process_callback);
  EXPECT_TRUE(notes.empty());
  notes.clear();

  // Process after the note position, but with a position offset to compensate.
  sequence.Process(6.0, 7.0, 1.5, process_callback);
  EXPECT_THAT(notes, ElementsAre(Pair(6.5, kNote)));
  notes.clear();

  // Set begin offset.
  sequence.SetBeginOffset(4.0);
  EXPECT_DOUBLE_EQ(sequence.GetBeginOffset(), 4.0);

  sequence.Process(1.0, 2.0, 0.0, process_callback);
  EXPECT_THAT(notes, ElementsAre(Pair(1.0, kNote)));
  notes.clear();

  // Set looping.
  sequence.SetLooping(true);
  EXPECT_TRUE(sequence.IsLooping());

  sequence.Process(1.0, 11.0, 1.0, process_callback);
  EXPECT_TRUE(notes.empty());
  notes.clear();

  // Set loop begin offset and length.
  sequence.SetLoopBeginOffset(2.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopBeginOffset(), 2.0);

  sequence.SetLoopLength(4.0);
  EXPECT_DOUBLE_EQ(sequence.GetLoopLength(), 4.0);

  sequence.Process(1.0, 11.0, 1.0, process_callback);
  EXPECT_THAT(notes,
              ElementsAre(Pair(2.0, kNote), Pair(6.0, kNote), Pair(10, kNote)));
  notes.clear();

  // Remove note.
  EXPECT_TRUE(IsOk(sequence.RemoveNote(kId)));
  EXPECT_THAT(GetStatusOrStatus(sequence.GetNote(kId)), Status::kNotFound);
  EXPECT_TRUE(sequence.IsEmpty());

  sequence.Process(1.0, 11.0, 1.0, process_callback);
  EXPECT_TRUE(notes.empty());
}

// Tests that multiple sequence notes are processed as expected.
TEST(SequenceTest, ProcessMultipleNotes) {
  Sequence sequence;
  EXPECT_TRUE(sequence.GetAllNotes().empty());
  EXPECT_TRUE(sequence.IsEmpty());

  // Add notes.
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(
        IsOk(sequence.AddNote(static_cast<Id>(i), static_cast<double>(i),
                              Note{.pitch = static_cast<float>(i + 1)})));
  }
  EXPECT_THAT(sequence.GetAllNotes(),
              ElementsAre(Pair(Pair(0.0, Id{0}), Note{.pitch = 1.0f}),
                          Pair(Pair(1.0, Id{1}), Note{.pitch = 2.0f}),
                          Pair(Pair(2.0, Id{2}), Note{.pitch = 3.0f}),
                          Pair(Pair(3.0, Id{3}), Note{.pitch = 4.0f})));
  EXPECT_FALSE(sequence.IsEmpty());

  std::vector<Sequence::NoteWithPosition> notes;
  const auto process_callback = [&](double position, const Note& note) {
    notes.emplace_back(position, note);
  };

  sequence.Process(0.0, 10.0, 0.0, process_callback);
  EXPECT_THAT(notes, ElementsAre(Pair(0.0, Note{.pitch = 1.0f}),
                                 Pair(1.0, Note{.pitch = 2.0f}),
                                 Pair(2.0, Note{.pitch = 3.0f}),
                                 Pair(3.0, Note{.pitch = 4.0f})));
  notes.clear();

  // Set looping with offset.
  sequence.SetBeginOffset(1.0);
  sequence.SetLooping(true);
  sequence.SetLoopBeginOffset(2.0);
  sequence.SetLoopLength(2.0);

  sequence.Process(0.0, 10.0, 0.0, process_callback);
  EXPECT_THAT(
      notes,
      ElementsAre(
          Pair(0.0, Note{.pitch = 2.0f}), Pair(1.0, Note{.pitch = 3.0f}),
          Pair(2.0, Note{.pitch = 4.0f}), Pair(3.0, Note{.pitch = 3.0f}),
          Pair(4.0, Note{.pitch = 4.0f}), Pair(5.0, Note{.pitch = 3.0f}),
          Pair(6.0, Note{.pitch = 4.0f}), Pair(7.0, Note{.pitch = 3.0f}),
          Pair(8.0, Note{.pitch = 4.0f}), Pair(9.0, Note{.pitch = 3.0f})));
  notes.clear();

  // Remove all notes.
  sequence.RemoveAllNotes();
  EXPECT_TRUE(sequence.GetAllNotes().empty());
  EXPECT_TRUE(sequence.IsEmpty());

  sequence.Process(0.0, 10.0, 0.0, process_callback);
  EXPECT_TRUE(notes.empty());
}

}  // namespace
}  // namespace barelyapi
