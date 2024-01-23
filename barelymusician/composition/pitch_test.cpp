#include "barelymusician/composition/pitch.h"

#include <array>

#include "barelymusician/barelymusician.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected note pitches are returned for given midi note numbers.
TEST(PitchTest, PitchFromMidi) {
  constexpr int kMidi = kMidiA0;
  constexpr Rational kPitch = kPitchA4;

  for (int i = 0; i < static_cast<int>(kSemitoneCount); ++i) {
    EXPECT_EQ(MidiFromPitch(kPitch + Rational(i, kSemitoneCount)), kMidi + i);
    EXPECT_EQ(PitchFromMidi(kMidi + i), kPitch + Rational(i, kSemitoneCount));

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_EQ(MidiFromPitch(PitchFromMidi(kMidi + i)), kMidi + i);
    EXPECT_EQ(PitchFromMidi(MidiFromPitch(kPitch + Rational(i, kSemitoneCount))),
              kPitch + Rational(i, kSemitoneCount));
  }
}

// Tests that expected note pitches are returned for a given arbitrary scale.
TEST(PitchTest, PitchFromScale) {
  constexpr int kOctaveRange = 2;
  const int scale_length = static_cast<int>(kPitchMajorScale.size());
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int index = octave * scale_length + i;
      const Rational expected_pitch = octave + kPitchMajorScale[i];
      EXPECT_EQ(PitchFromScale(kPitchMajorScale, index), expected_pitch);
    }
  }
}

}  // namespace
}  // namespace barely
