#ifndef BARELYMUSICIAN_COMPOSITION_PITCH_H_
#define BARELYMUSICIAN_COMPOSITION_PITCH_H_

#include <array>
#include <span>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/rational.h"

namespace barely {

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr int kSemitoneCount = 12;

/// Common note pitches.
inline constexpr Rational kPitchC4 = Rational(-10, kSemitoneCount);
inline constexpr Rational kPitchCsharp4 = Rational(-9, kSemitoneCount);
inline constexpr Rational kPitchD4 = Rational(-8, kSemitoneCount);
inline constexpr Rational kPitchDsharp4 = Rational(-7, kSemitoneCount);
inline constexpr Rational kPitchE4 = Rational(-5, kSemitoneCount);
inline constexpr Rational kPitchF4 = Rational(-4, kSemitoneCount);
inline constexpr Rational kPitchFsharp4 = Rational(-3, kSemitoneCount);
inline constexpr Rational kPitchG4 = Rational(-2, kSemitoneCount);
inline constexpr Rational kPitchGsharp4 = Rational(-1, kSemitoneCount);
inline constexpr Rational kPitchA4 = 0;
inline constexpr Rational kPitchAsharp4 = Rational(1, kSemitoneCount);
inline constexpr Rational kPitchB4 = Rational(2, kSemitoneCount);
inline constexpr Rational kPitchKick = kPitchC4;
inline constexpr Rational kPitchSnare = kPitchD4;
inline constexpr Rational kPitchHihatClosed = kPitchE4;
inline constexpr Rational kPitchHihatOpen = kPitchF4;

/// Common musical scales.
inline constexpr std::array<Rational, 7> kPitchMajorScale = {
    Rational(0, kSemitoneCount),  Rational(2, kSemitoneCount), Rational(4, kSemitoneCount),
    Rational(5, kSemitoneCount),  Rational(7, kSemitoneCount), Rational(9, kSemitoneCount),
    Rational(11, kSemitoneCount),
};
inline constexpr std::array<Rational, 7> kPitchNaturalMinorScale = {
    Rational(0, kSemitoneCount),  Rational(2, kSemitoneCount), Rational(3, kSemitoneCount),
    Rational(5, kSemitoneCount),  Rational(7, kSemitoneCount), Rational(8, kSemitoneCount),
    Rational(10, kSemitoneCount),
};
inline constexpr std::array<Rational, 7> kPitchHarmonicMinorScale = {
    Rational(0, kSemitoneCount),  Rational(2, kSemitoneCount), Rational(3, kSemitoneCount),
    Rational(5, kSemitoneCount),  Rational(7, kSemitoneCount), Rational(8, kSemitoneCount),
    Rational(11, kSemitoneCount),
};

// Midi note number for A4 (middle A).
inline constexpr int kMidiA0 = 69;

/// Returns the correspnding midi note number for a given pitch.
///
/// @param pitch Note pitch.
/// @return Midi note number.
int MidiFromPitch(Rational pitch) noexcept;

/// Returns the correspnding note pitch for a given midi note number.
///
/// @param midi Midi note number.
/// @return Note pitch.
Rational PitchFromMidi(int midi) noexcept;

/// Returns the corresponding note pitch for a given scale and index.
///
/// @param scale Cumulative scale intervals of an octave in an increasing order.
/// @param index Scale index.
/// @return Note pitch.
Rational PitchFromScale(std::span<const Rational> scale, int index) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_PITCH_H_
