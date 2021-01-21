#include "barelymusician/engine/track.h"

#include <variant>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the track stores a single note as expected.
TEST(TrackTest, SingleNote) {
  const double kPosition = 4.0f;
  const double kDuration = 0.5f;
  const float kPitch = 2.5f;
  const float kIntensity = 0.25f;

  Track track;
  EXPECT_TRUE(track.IsEmpty());

  // Add note.
  const int event_id =
      track.AddNoteEvent(kPosition, kDuration, kPitch, kIntensity);
  EXPECT_FALSE(track.IsEmpty());

  int num_note_ons = 0;
  int num_note_offs = 0;
  track.ForEachEventInRange(
      kPosition, kPosition + kDuration + 1.0,
      [&](double position, const InstrumentData& data) {
        if (std::holds_alternative<NoteOn>(data)) {
          ++num_note_ons;
          const auto note_on = std::get<NoteOn>(data);
          EXPECT_DOUBLE_EQ(position, kPosition);
          EXPECT_FLOAT_EQ(note_on.pitch, kPitch);
          EXPECT_FLOAT_EQ(note_on.intensity, kIntensity);
        } else {
          ASSERT_TRUE(std::holds_alternative<NoteOff>(data));
          ++num_note_offs;
          const auto note_off = std::get<NoteOff>(data);
          EXPECT_DOUBLE_EQ(position, kPosition + kDuration);
          EXPECT_FLOAT_EQ(note_off.pitch, kPitch);
        }
      });
  EXPECT_EQ(num_note_ons, 1);
  EXPECT_EQ(num_note_offs, 1);

  EXPECT_TRUE(track.RemoveEvent(event_id));
  EXPECT_TRUE(track.IsEmpty());

  // Second remove attempt should fail since the note no longer exists.
  EXPECT_FALSE(track.RemoveEvent(event_id));
}

// Tests that the track stores multiple notes as expected.
TEST(TrackTest, MultipleNotes) {
  const int kNumNotes = 10;
  const float kIntensity = 0.25f;

  Track track;
  EXPECT_TRUE(track.IsEmpty());

  // Add notes.
  for (int i = 0; i < kNumNotes; ++i) {
    track.AddNoteEvent(static_cast<double>(i), 1.0, static_cast<float>(i),
                       kIntensity);
  }
  EXPECT_FALSE(track.IsEmpty());

  int num_note_ons = 0;
  int num_note_offs = 0;
  track.ForEachEventInRange(
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
          EXPECT_DOUBLE_EQ(position, static_cast<double>(note_off.pitch) + 1.0);
          ++num_note_offs;
        }
      });
  EXPECT_EQ(num_note_ons, kNumNotes);
  // Last note off should be out of range.
  EXPECT_EQ(num_note_offs, kNumNotes - 1);

  track.RemoveAllEvents();
  EXPECT_TRUE(track.IsEmpty());
}

// Tests that removing arbitrary notes in range works as expected.
TEST(TrackTest, RemoveAllEventsInRange) {
  const float kNotePitch = 10.0f;
  const float kNoteIntensity = 1.0f;

  Track track;
  EXPECT_TRUE(track.IsEmpty());

  track.AddNoteEvent(2.0, 6.0, kNotePitch, kNoteIntensity);
  track.AddNoteEvent(4.0, 3.0, kNotePitch, kNoteIntensity);
  EXPECT_FALSE(track.IsEmpty());

  track.RemoveAllEventsInRange(0.0, 2.0);
  EXPECT_FALSE(track.IsEmpty());
  track.RemoveAllEventsInRange(8.0, 20.0);
  EXPECT_FALSE(track.IsEmpty());
  track.RemoveAllEventsInRange(3.0, 5.0);
  EXPECT_TRUE(track.IsEmpty());
}

}  // namespace
}  // namespace barelyapi
