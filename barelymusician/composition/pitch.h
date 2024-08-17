#ifndef BARELYMUSICIAN_COMPOSITION_PITCH_H_
#define BARELYMUSICIAN_COMPOSITION_PITCH_H_

#include <array>
#include <span>

namespace barely {

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr double kSemitoneCount = 12.0;

/// Semitone pitch intervals of an octave.
inline constexpr std::array<double, static_cast<int>(kSemitoneCount)> kSemitoneRatios = {
    1.0,
    1.0594630943592953,  // std::pow(2.0, 1.0 / 12.0)
    1.122462048309373,   // std::pow(2.0, 2.0 / 12.0)
    1.189207115002721,   // std::pow(2.0, 3.0 / 12.0)
    1.2599210498948732,  // std::pow(2.0, 4.0 / 12.0)
    1.3348398541700344,  // std::pow(2.0, 5.0 / 12.0)
    1.4142135623730951,  // std::pow(2.0, 6.0 / 12.0)
    1.4983070768766815,  // std::pow(2.0, 7.0 / 12.0)
    1.5874010519681994,  // std::pow(2.0, 8.0 / 12.0)
    1.681792830507429,   // std::pow(2.0, 9.0 / 12.0)
    1.7817974362806785,  // std::pow(2.0, 10.0 / 12.0)
    1.8877486253633868,  // std::pow(2.0, 11.0 / 12.0)
};

/// Common musical scales.
inline constexpr std::array<double, 7> kPitchMajorScale = {
    kSemitoneRatios[0], kSemitoneRatios[2], kSemitoneRatios[4],  kSemitoneRatios[5],
    kSemitoneRatios[7], kSemitoneRatios[9], kSemitoneRatios[11],
};
inline constexpr std::array<double, 7> kPitchNaturalMinorScale = {
    kSemitoneRatios[0], kSemitoneRatios[2], kSemitoneRatios[3],  kSemitoneRatios[5],
    kSemitoneRatios[7], kSemitoneRatios[8], kSemitoneRatios[10],
};
inline constexpr std::array<double, 7> kPitchHarmonicMinorScale = {
    kSemitoneRatios[0], kSemitoneRatios[2], kSemitoneRatios[3],  kSemitoneRatios[5],
    kSemitoneRatios[7], kSemitoneRatios[8], kSemitoneRatios[11],
};

// Middle A (A4) reference frequency.
inline constexpr double kFrequencyA4 = 440.0;

/// Common note values.
inline constexpr double kNoteA0 = kFrequencyA4 / 16.0;
inline constexpr double kNoteAsharp0 = kNoteA0 * kSemitoneRatios[1];
inline constexpr double kNoteB0 = kNoteA0 * kSemitoneRatios[2];
inline constexpr double kNoteC1 = kNoteA0 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp1 = kNoteA0 * kSemitoneRatios[4];
inline constexpr double kNoteD1 = kNoteA0 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp1 = kNoteA0 * kSemitoneRatios[6];
inline constexpr double kNoteE1 = kNoteA0 * kSemitoneRatios[7];
inline constexpr double kNoteF1 = kNoteA0 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp1 = kNoteA0 * kSemitoneRatios[9];
inline constexpr double kNoteG1 = kNoteA0 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp1 = kNoteA0 * kSemitoneRatios[11];
inline constexpr double kNoteA1 = kFrequencyA4 / 8.0;
inline constexpr double kNoteAsharp1 = kNoteA1 * kSemitoneRatios[1];
inline constexpr double kNoteB1 = kNoteA1 * kSemitoneRatios[2];
inline constexpr double kNoteC2 = kNoteA1 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp2 = kNoteA1 * kSemitoneRatios[4];
inline constexpr double kNoteD2 = kNoteA1 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp2 = kNoteA1 * kSemitoneRatios[6];
inline constexpr double kNoteE2 = kNoteA1 * kSemitoneRatios[7];
inline constexpr double kNoteF2 = kNoteA1 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp2 = kNoteA1 * kSemitoneRatios[9];
inline constexpr double kNoteG2 = kNoteA1 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp2 = kNoteA1 * kSemitoneRatios[11];
inline constexpr double kNoteA2 = kFrequencyA4 / 4.0;
inline constexpr double kNoteAsharp2 = kNoteA2 * kSemitoneRatios[1];
inline constexpr double kNoteB2 = kNoteA2 * kSemitoneRatios[2];
inline constexpr double kNoteC3 = kNoteA2 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp3 = kNoteA2 * kSemitoneRatios[4];
inline constexpr double kNoteD3 = kNoteA2 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp3 = kNoteA2 * kSemitoneRatios[6];
inline constexpr double kNoteE3 = kNoteA2 * kSemitoneRatios[7];
inline constexpr double kNoteF3 = kNoteA2 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp3 = kNoteA2 * kSemitoneRatios[9];
inline constexpr double kNoteG3 = kNoteA2 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp3 = kNoteA2 * kSemitoneRatios[11];
inline constexpr double kNoteA3 = kFrequencyA4 / 2.0;
inline constexpr double kNoteAsharp3 = kNoteA3 * kSemitoneRatios[1];
inline constexpr double kNoteB3 = kNoteA3 * kSemitoneRatios[2];
inline constexpr double kNoteC4 = kNoteA3 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp4 = kNoteA3 * kSemitoneRatios[4];
inline constexpr double kNoteD4 = kNoteA3 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp4 = kNoteA3 * kSemitoneRatios[6];
inline constexpr double kNoteE4 = kNoteA3 * kSemitoneRatios[7];
inline constexpr double kNoteF4 = kNoteA3 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp4 = kNoteA3 * kSemitoneRatios[9];
inline constexpr double kNoteG4 = kNoteA3 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp4 = kNoteA3 * kSemitoneRatios[11];
inline constexpr double kNoteA4 = kFrequencyA4;
inline constexpr double kNoteAsharp4 = kNoteA4 * kSemitoneRatios[1];
inline constexpr double kNoteB4 = kNoteA4 * kSemitoneRatios[2];
inline constexpr double kNoteC5 = kNoteA4 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp5 = kNoteA4 * kSemitoneRatios[4];
inline constexpr double kNoteD5 = kNoteA4 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp5 = kNoteA4 * kSemitoneRatios[6];
inline constexpr double kNoteE5 = kNoteA4 * kSemitoneRatios[7];
inline constexpr double kNoteF5 = kNoteA4 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp5 = kNoteA4 * kSemitoneRatios[9];
inline constexpr double kNoteG5 = kNoteA4 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp5 = kNoteA4 * kSemitoneRatios[11];
inline constexpr double kNoteA5 = kNoteA4 * 2.0;
inline constexpr double kNoteAsharp5 = kNoteA5 * kSemitoneRatios[1];
inline constexpr double kNoteB5 = kNoteA5 * kSemitoneRatios[2];
inline constexpr double kNoteC6 = kNoteA5 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp6 = kNoteA5 * kSemitoneRatios[4];
inline constexpr double kNoteD6 = kNoteA5 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp6 = kNoteA5 * kSemitoneRatios[6];
inline constexpr double kNoteE6 = kNoteA5 * kSemitoneRatios[7];
inline constexpr double kNoteF6 = kNoteA5 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp6 = kNoteA5 * kSemitoneRatios[9];
inline constexpr double kNoteG6 = kNoteA5 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp6 = kNoteA5 * kSemitoneRatios[11];
inline constexpr double kNoteA6 = 2.0;
inline constexpr double kNoteAsharp6 = kNoteA6 * kSemitoneRatios[1];
inline constexpr double kNoteB6 = kNoteA6 * kSemitoneRatios[2];
inline constexpr double kNoteC7 = kNoteA6 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp7 = kNoteA6 * kSemitoneRatios[4];
inline constexpr double kNoteD7 = kNoteA6 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp7 = kNoteA6 * kSemitoneRatios[6];
inline constexpr double kNoteE7 = kNoteA6 * kSemitoneRatios[7];
inline constexpr double kNoteF7 = kNoteA6 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp7 = kNoteA6 * kSemitoneRatios[9];
inline constexpr double kNoteG7 = kNoteA6 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp7 = kNoteA6 * kSemitoneRatios[11];
inline constexpr double kNoteA7 = 3.0;
inline constexpr double kNoteAsharp7 = kNoteA7 * kSemitoneRatios[1];
inline constexpr double kNoteB7 = kNoteA7 * kSemitoneRatios[2];
inline constexpr double kNoteC8 = kNoteA7 * kSemitoneRatios[3];
inline constexpr double kNoteCsharp8 = kNoteA7 * kSemitoneRatios[4];
inline constexpr double kNoteD8 = kNoteA7 * kSemitoneRatios[5];
inline constexpr double kNoteDsharp8 = kNoteA7 * kSemitoneRatios[6];
inline constexpr double kNoteE8 = kNoteA7 * kSemitoneRatios[7];
inline constexpr double kNoteF8 = kNoteA7 * kSemitoneRatios[8];
inline constexpr double kNoteFsharp8 = kNoteA7 * kSemitoneRatios[9];
inline constexpr double kNoteG8 = kNoteA7 * kSemitoneRatios[10];
inline constexpr double kNoteGsharp8 = kNoteA7 * kSemitoneRatios[11];
inline constexpr double kNoteA8 = 4.0;
inline constexpr double kNoteKick = kNoteC3;
inline constexpr double kNoteSnare = kNoteD3;
inline constexpr double kNoteHihatClosed = kNoteE3;
inline constexpr double kNoteHihatOpen = kNoteF3;

/// Middle note values.
inline constexpr double kNoteC = kNoteC4;
inline constexpr double kNoteCsharp = kNoteCsharp4;
inline constexpr double kNoteD = kNoteD4;
inline constexpr double kNoteDsharp = kNoteDsharp4;
inline constexpr double kNoteE = kNoteE4;
inline constexpr double kNoteF = kNoteF4;
inline constexpr double kNoteFsharp = kNoteFsharp4;
inline constexpr double kNoteG = kNoteG4;
inline constexpr double kNoteGsharp = kNoteGsharp4;
inline constexpr double kNoteA = kNoteA4;
inline constexpr double kNoteASharp = kNoteAsharp4;
inline constexpr double kNoteB = kNoteB4;

/// Returns the corresponding note pitch for a given scale and index.
///
/// @param scale Cumulative scale intervals of an octave in an increasing order.
/// @param index Scale index.
/// @return Note pitch.
double PitchFromScale(std::span<const double> scale, int index) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_PITCH_H_
