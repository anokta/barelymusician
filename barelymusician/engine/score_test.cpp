#include "barelymusician/engine/score.h"

#include <variant>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the score stores a single note as expected.
TEST(ScoreTest, SingleNote) {
  const double kPosition = 4.0f;
  const double kDuration = 0.5f;
  const float kPitch = 2.5f;
  const float kIntensity = 0.25f;

  Score score;
  EXPECT_TRUE(score.IsEmpty());

  // Add note.
  const int event_id =
      score.AddNoteEvent(kPosition, kPosition + kDuration, kPitch, kIntensity);
  EXPECT_FALSE(score.IsEmpty());

  score.ForEachEventInRange(
      kPosition, kPosition + kDuration,
      [&](double position, const InstrumentData& data) {
        if (std::holds_alternative<NoteOn>(data)) {
          const auto note_on = std::get<NoteOn>(data);
          EXPECT_DOUBLE_EQ(position, kPosition);
          EXPECT_FLOAT_EQ(note_on.pitch, kPitch);
          EXPECT_FLOAT_EQ(note_on.intensity, kIntensity);
        } else {
          ASSERT_TRUE(std::holds_alternative<NoteOff>(data));
          const auto note_off = std::get<NoteOff>(data);
          EXPECT_DOUBLE_EQ(position, kPosition + kDuration);
          EXPECT_FLOAT_EQ(note_off.pitch, kPitch);
        }
      });

  EXPECT_TRUE(score.RemoveEvent(event_id));
  EXPECT_TRUE(score.IsEmpty());

  // Second remove attempt should fail since the note no longer exists.
  EXPECT_FALSE(score.RemoveEvent(event_id));
}

// Tests that the score stores multiple notes as expected.
TEST(ScoreTest, MultipleNotes) {
  const int kNumNotes = 10;
  const double kDuration = 0.5;
  const float kIntensity = 0.25f;

  Score score;
  EXPECT_TRUE(score.IsEmpty());

  // Add notes.
  for (int i = 0; i < kNumNotes; ++i) {
    score.AddNoteEvent(static_cast<double>(i), kDuration, static_cast<float>(i),
                       kIntensity);
  }
  EXPECT_FALSE(score.IsEmpty());

  int num_note_ons = 0;
  int num_note_offs = 0;
  score.ForEachEventInRange(
      0.0, static_cast<double>(kNumNotes),
      [&](double position, const InstrumentData& data) {
        if (std::holds_alternative<NoteOn>(data)) {
          const auto note_on = std::get<NoteOn>(data);
          EXPECT_DOUBLE_EQ(position, static_cast<double>(note_on.pitch));
          EXPECT_FLOAT_EQ(note_on.intensity, kIntensity);
          ++num_note_ons;
        } else {
          ASSERT_TRUE(std::holds_alternative<NoteOff>(data));
          const auto note_off = std::get<NoteOff>(data);
          EXPECT_DOUBLE_EQ(position,
                           static_cast<double>(note_off.pitch) + kDuration);
          ++num_note_offs;
        }
      });
  EXPECT_EQ(num_note_ons, kNumNotes);
  EXPECT_EQ(num_note_offs, kNumNotes);

  score.RemoveAllEvents();
  EXPECT_TRUE(score.IsEmpty());
}

}  // namespace
}  // namespace barelyapi
