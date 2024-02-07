#include "barelymusician/composition/pitch.h"

#include <array>

#include "gtest/gtest.h"

namespace barely {
namespace {

// Tests that expected note pitches are returned for given midi note numbers.
TEST(PitchTest, PitchFromMidi) {
  constexpr std::array<int, static_cast<int>(kSemitoneCount)> kMidis = {
      kMidiA0,     kMidiA0 + 1, kMidiA0 + 2, kMidiA0 + 3, kMidiA0 + 4,  kMidiA0 + 5,
      kMidiA0 + 6, kMidiA0 + 7, kMidiA0 + 8, kMidiA0 + 9, kMidiA0 + 10, kMidiA0 + 11,
  };
  constexpr std::array<double, static_cast<int>(kSemitoneCount)> kPitches = {
      kPitchA4,      kPitchAsharp4, kPitchB4, kPitchC5,      kPitchCsharp5, kPitchD5,
      kPitchDsharp5, kPitchE5,      kPitchF5, kPitchFsharp5, kPitchG5,      kPitchGsharp5,
  };

  for (int i = 0; i < static_cast<int>(kSemitoneCount); ++i) {
    EXPECT_DOUBLE_EQ(MidiFromPitch(kPitches[i]), kMidis[i]);
    EXPECT_DOUBLE_EQ(PitchFromMidi(kMidis[i]), kPitches[i]);

    // Verify that the back and forth conversions do not mutate the value.
    EXPECT_DOUBLE_EQ(MidiFromPitch(PitchFromMidi(kMidis[i])), kMidis[i]);
    EXPECT_DOUBLE_EQ(PitchFromMidi(MidiFromPitch(kPitches[i])), kPitches[i]);
  }
}

// Tests that expected note pitches are returned for a given arbitrary scale.
TEST(PitchTest, PitchFromScale) {
  constexpr int kOctaveRange = 2;
  const int scale_length = static_cast<int>(kPitchMajorScale.size());
  for (int octave = -kOctaveRange; octave <= kOctaveRange; ++octave) {
    for (int i = 0; i < scale_length; ++i) {
      const int index = octave * scale_length + i;
      const double expected_pitch = static_cast<double>(octave) + kPitchMajorScale[i];
      EXPECT_DOUBLE_EQ(PitchFromScale(kPitchMajorScale, index), expected_pitch);
    }
  }
}

}  // namespace
}  // namespace barely
