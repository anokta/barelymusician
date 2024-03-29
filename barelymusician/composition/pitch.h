#ifndef BARELYMUSICIAN_COMPOSITION_PITCH_H_
#define BARELYMUSICIAN_COMPOSITION_PITCH_H_

#include <array>
#include <span>

namespace barely {

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr double kSemitoneCount = 12.0;

/// Semitone pitch intervals of an octave.
inline constexpr std::array<double, static_cast<int>(kSemitoneCount)> kPitchSemitones = {
    0.0,
    1.0 / kSemitoneCount,
    2.0 / kSemitoneCount,
    3.0 / kSemitoneCount,
    4.0 / kSemitoneCount,
    5.0 / kSemitoneCount,
    6.0 / kSemitoneCount,
    7.0 / kSemitoneCount,
    8.0 / kSemitoneCount,
    9.0 / kSemitoneCount,
    10.0 / kSemitoneCount,
    11.0 / kSemitoneCount,
};

/// Common musical scales.
inline constexpr std::array<double, 7> kPitchMajorScale = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[4],  kPitchSemitones[5],
    kPitchSemitones[7], kPitchSemitones[9], kPitchSemitones[11],
};
inline constexpr std::array<double, 7> kPitchNaturalMinorScale = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[3],  kPitchSemitones[5],
    kPitchSemitones[7], kPitchSemitones[8], kPitchSemitones[10],
};
inline constexpr std::array<double, 7> kPitchHarmonicMinorScale = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[3],  kPitchSemitones[5],
    kPitchSemitones[7], kPitchSemitones[8], kPitchSemitones[11],
};

/// Common note pitches.
inline constexpr double kPitchA0 = -4.0;
inline constexpr double kPitchAsharp0 = kPitchA0 + kPitchSemitones[1];
inline constexpr double kPitchB0 = kPitchA0 + kPitchSemitones[2];
inline constexpr double kPitchC1 = kPitchA0 + kPitchSemitones[3];
inline constexpr double kPitchCsharp1 = kPitchA0 + kPitchSemitones[4];
inline constexpr double kPitchD1 = kPitchA0 + kPitchSemitones[5];
inline constexpr double kPitchDsharp1 = kPitchA0 + kPitchSemitones[6];
inline constexpr double kPitchE1 = kPitchA0 + kPitchSemitones[7];
inline constexpr double kPitchF1 = kPitchA0 + kPitchSemitones[8];
inline constexpr double kPitchFsharp1 = kPitchA0 + kPitchSemitones[9];
inline constexpr double kPitchG1 = kPitchA0 + kPitchSemitones[10];
inline constexpr double kPitchGsharp1 = kPitchA0 + kPitchSemitones[11];
inline constexpr double kPitchA1 = -3.0;
inline constexpr double kPitchAsharp1 = kPitchA1 + kPitchSemitones[1];
inline constexpr double kPitchB1 = kPitchA1 + kPitchSemitones[2];
inline constexpr double kPitchC2 = kPitchA1 + kPitchSemitones[3];
inline constexpr double kPitchCsharp2 = kPitchA1 + kPitchSemitones[4];
inline constexpr double kPitchD2 = kPitchA1 + kPitchSemitones[5];
inline constexpr double kPitchDsharp2 = kPitchA1 + kPitchSemitones[6];
inline constexpr double kPitchE2 = kPitchA1 + kPitchSemitones[7];
inline constexpr double kPitchF2 = kPitchA1 + kPitchSemitones[8];
inline constexpr double kPitchFsharp2 = kPitchA1 + kPitchSemitones[9];
inline constexpr double kPitchG2 = kPitchA1 + kPitchSemitones[10];
inline constexpr double kPitchGsharp2 = kPitchA1 + kPitchSemitones[11];
inline constexpr double kPitchA2 = -2.0;
inline constexpr double kPitchAsharp2 = kPitchA2 + kPitchSemitones[1];
inline constexpr double kPitchB2 = kPitchA2 + kPitchSemitones[2];
inline constexpr double kPitchC3 = kPitchA2 + kPitchSemitones[3];
inline constexpr double kPitchCsharp3 = kPitchA2 + kPitchSemitones[4];
inline constexpr double kPitchD3 = kPitchA2 + kPitchSemitones[5];
inline constexpr double kPitchDsharp3 = kPitchA2 + kPitchSemitones[6];
inline constexpr double kPitchE3 = kPitchA2 + kPitchSemitones[7];
inline constexpr double kPitchF3 = kPitchA2 + kPitchSemitones[8];
inline constexpr double kPitchFsharp3 = kPitchA2 + kPitchSemitones[9];
inline constexpr double kPitchG3 = kPitchA2 + kPitchSemitones[10];
inline constexpr double kPitchGsharp3 = kPitchA2 + kPitchSemitones[11];
inline constexpr double kPitchA3 = -1.0;
inline constexpr double kPitchAsharp3 = kPitchA3 + kPitchSemitones[1];
inline constexpr double kPitchB3 = kPitchA3 + kPitchSemitones[2];
inline constexpr double kPitchC4 = kPitchA3 + kPitchSemitones[3];
inline constexpr double kPitchCsharp4 = kPitchA3 + kPitchSemitones[4];
inline constexpr double kPitchD4 = kPitchA3 + kPitchSemitones[5];
inline constexpr double kPitchDsharp4 = kPitchA3 + kPitchSemitones[6];
inline constexpr double kPitchE4 = kPitchA3 + kPitchSemitones[7];
inline constexpr double kPitchF4 = kPitchA3 + kPitchSemitones[8];
inline constexpr double kPitchFsharp4 = kPitchA3 + kPitchSemitones[9];
inline constexpr double kPitchG4 = kPitchA3 + kPitchSemitones[10];
inline constexpr double kPitchGsharp4 = kPitchA3 + kPitchSemitones[11];
inline constexpr double kPitchA4 = 0.0;
inline constexpr double kPitchAsharp4 = kPitchSemitones[1];
inline constexpr double kPitchB4 = kPitchSemitones[2];
inline constexpr double kPitchC5 = kPitchSemitones[3];
inline constexpr double kPitchCsharp5 = kPitchSemitones[4];
inline constexpr double kPitchD5 = kPitchSemitones[5];
inline constexpr double kPitchDsharp5 = kPitchSemitones[6];
inline constexpr double kPitchE5 = kPitchSemitones[7];
inline constexpr double kPitchF5 = kPitchSemitones[8];
inline constexpr double kPitchFsharp5 = kPitchSemitones[9];
inline constexpr double kPitchG5 = kPitchSemitones[10];
inline constexpr double kPitchGsharp5 = kPitchSemitones[11];
inline constexpr double kPitchA5 = 1.0;
inline constexpr double kPitchAsharp5 = kPitchA5 + kPitchSemitones[1];
inline constexpr double kPitchB5 = kPitchA5 + kPitchSemitones[2];
inline constexpr double kPitchC6 = kPitchA5 + kPitchSemitones[3];
inline constexpr double kPitchCsharp6 = kPitchA5 + kPitchSemitones[4];
inline constexpr double kPitchD6 = kPitchA5 + kPitchSemitones[5];
inline constexpr double kPitchDsharp6 = kPitchA5 + kPitchSemitones[6];
inline constexpr double kPitchE6 = kPitchA5 + kPitchSemitones[7];
inline constexpr double kPitchF6 = kPitchA5 + kPitchSemitones[8];
inline constexpr double kPitchFsharp6 = kPitchA5 + kPitchSemitones[9];
inline constexpr double kPitchG6 = kPitchA5 + kPitchSemitones[10];
inline constexpr double kPitchGsharp6 = kPitchA5 + kPitchSemitones[11];
inline constexpr double kPitchA6 = 2.0;
inline constexpr double kPitchAsharp6 = kPitchA6 + kPitchSemitones[1];
inline constexpr double kPitchB6 = kPitchA6 + kPitchSemitones[2];
inline constexpr double kPitchC7 = kPitchA6 + kPitchSemitones[3];
inline constexpr double kPitchCsharp7 = kPitchA6 + kPitchSemitones[4];
inline constexpr double kPitchD7 = kPitchA6 + kPitchSemitones[5];
inline constexpr double kPitchDsharp7 = kPitchA6 + kPitchSemitones[6];
inline constexpr double kPitchE7 = kPitchA6 + kPitchSemitones[7];
inline constexpr double kPitchF7 = kPitchA6 + kPitchSemitones[8];
inline constexpr double kPitchFsharp7 = kPitchA6 + kPitchSemitones[9];
inline constexpr double kPitchG7 = kPitchA6 + kPitchSemitones[10];
inline constexpr double kPitchGsharp7 = kPitchA6 + kPitchSemitones[11];
inline constexpr double kPitchA7 = 3.0;
inline constexpr double kPitchAsharp7 = kPitchA7 + kPitchSemitones[1];
inline constexpr double kPitchB7 = kPitchA7 + kPitchSemitones[2];
inline constexpr double kPitchC8 = kPitchA7 + kPitchSemitones[3];
inline constexpr double kPitchCsharp8 = kPitchA7 + kPitchSemitones[4];
inline constexpr double kPitchD8 = kPitchA7 + kPitchSemitones[5];
inline constexpr double kPitchDsharp8 = kPitchA7 + kPitchSemitones[6];
inline constexpr double kPitchE8 = kPitchA7 + kPitchSemitones[7];
inline constexpr double kPitchF8 = kPitchA7 + kPitchSemitones[8];
inline constexpr double kPitchFsharp8 = kPitchA7 + kPitchSemitones[9];
inline constexpr double kPitchG8 = kPitchA7 + kPitchSemitones[10];
inline constexpr double kPitchGsharp8 = kPitchA7 + kPitchSemitones[11];
inline constexpr double kPitchA8 = 4.0;
inline constexpr double kPitchKick = kPitchC3;
inline constexpr double kPitchSnare = kPitchD3;
inline constexpr double kPitchHihatClosed = kPitchE3;
inline constexpr double kPitchHihatOpen = kPitchF3;

/// Middle note pitches.
inline constexpr double kPitchC = kPitchC4;
inline constexpr double kPitchCsharp = kPitchCsharp4;
inline constexpr double kPitchD = kPitchD4;
inline constexpr double kPitchDsharp = kPitchDsharp4;
inline constexpr double kPitchE = kPitchE4;
inline constexpr double kPitchF = kPitchF4;
inline constexpr double kPitchFsharp = kPitchFsharp4;
inline constexpr double kPitchG = kPitchG4;
inline constexpr double kPitchGsharp = kPitchGsharp4;
inline constexpr double kPitchA = kPitchA4;
inline constexpr double kPitchASharp = kPitchAsharp4;
inline constexpr double kPitchB = kPitchB4;

/// Returns the corresponding note pitch for a given scale and index.
///
/// @param scale Cumulative scale intervals of an octave in an increasing order.
/// @param index Scale index.
/// @return Note pitch.
double PitchFromScale(std::span<const double> scale, int index) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPOSITION_PITCH_H_
