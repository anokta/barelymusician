#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_

#include <span>
#include <variant>

namespace barelyapi {

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr float kNumSemitones = 12.0f;

/// Semitone pitch intervals of an octave.
inline constexpr float kPitchSemitones[12] = {0.0f,
                                              1.0f / kNumSemitones,
                                              2.0f / kNumSemitones,
                                              3.0f / kNumSemitones,
                                              4.0f / kNumSemitones,
                                              5.0f / kNumSemitones,
                                              6.0f / kNumSemitones,
                                              7.0f / kNumSemitones,
                                              8.0f / kNumSemitones,
                                              9.0f / kNumSemitones,
                                              10.0f / kNumSemitones,
                                              11.0f / kNumSemitones};

/// Common musical scales.
inline constexpr float kPitchMajorScale[7] = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[4],
    kPitchSemitones[5], kPitchSemitones[7], kPitchSemitones[9],
    kPitchSemitones[11]};
inline constexpr float kPitchNaturalMinorScale[7] = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[3],
    kPitchSemitones[5], kPitchSemitones[7], kPitchSemitones[8],
    kPitchSemitones[10]};
inline constexpr float kPitchHarmonicMinorScale[7] = {
    kPitchSemitones[0], kPitchSemitones[2], kPitchSemitones[3],
    kPitchSemitones[5], kPitchSemitones[7], kPitchSemitones[8],
    kPitchSemitones[11]};

/// Common note pitches.
inline constexpr float kPitchA0 = -4.0f;
inline constexpr float kPitchAsharp0 = kPitchA0 + kPitchSemitones[1];
inline constexpr float kPitchB0 = kPitchA0 + kPitchSemitones[2];
inline constexpr float kPitchC0 = kPitchA0 + kPitchSemitones[3];
inline constexpr float kPitchCsharp0 = kPitchA0 + kPitchSemitones[4];
inline constexpr float kPitchD0 = kPitchA0 + kPitchSemitones[5];
inline constexpr float kPitchDsharp0 = kPitchA0 + kPitchSemitones[6];
inline constexpr float kPitchE0 = kPitchA0 + kPitchSemitones[7];
inline constexpr float kPitchF0 = kPitchA0 + kPitchSemitones[8];
inline constexpr float kPitchFsharp0 = kPitchA0 + kPitchSemitones[9];
inline constexpr float kPitchG0 = kPitchA0 + kPitchSemitones[10];
inline constexpr float kPitchGsharp0 = kPitchA0 + kPitchSemitones[11];
inline constexpr float kPitchA1 = -3.0f;
inline constexpr float kPitchAsharp1 = kPitchA1 + kPitchSemitones[1];
inline constexpr float kPitchB1 = kPitchA1 + kPitchSemitones[2];
inline constexpr float kPitchC1 = kPitchA1 + kPitchSemitones[3];
inline constexpr float kPitchCsharp1 = kPitchA1 + kPitchSemitones[4];
inline constexpr float kPitchD1 = kPitchA1 + kPitchSemitones[5];
inline constexpr float kPitchDsharp1 = kPitchA1 + kPitchSemitones[6];
inline constexpr float kPitchE1 = kPitchA1 + kPitchSemitones[7];
inline constexpr float kPitchF1 = kPitchA1 + kPitchSemitones[8];
inline constexpr float kPitchFsharp1 = kPitchA1 + kPitchSemitones[9];
inline constexpr float kPitchG1 = kPitchA1 + kPitchSemitones[10];
inline constexpr float kPitchGsharp1 = kPitchA1 + kPitchSemitones[11];
inline constexpr float kPitchA2 = -2.0f;
inline constexpr float kPitchAsharp2 = kPitchA2 + kPitchSemitones[1];
inline constexpr float kPitchB2 = kPitchA2 + kPitchSemitones[2];
inline constexpr float kPitchC2 = kPitchA2 + kPitchSemitones[3];
inline constexpr float kPitchCsharp2 = kPitchA2 + kPitchSemitones[4];
inline constexpr float kPitchD2 = kPitchA2 + kPitchSemitones[5];
inline constexpr float kPitchDsharp2 = kPitchA2 + kPitchSemitones[6];
inline constexpr float kPitchE2 = kPitchA2 + kPitchSemitones[7];
inline constexpr float kPitchF2 = kPitchA2 + kPitchSemitones[8];
inline constexpr float kPitchFsharp2 = kPitchA2 + kPitchSemitones[9];
inline constexpr float kPitchG2 = kPitchA2 + kPitchSemitones[10];
inline constexpr float kPitchGsharp2 = kPitchA2 + kPitchSemitones[11];
inline constexpr float kPitchA3 = -1.0f;
inline constexpr float kPitchAsharp3 = kPitchA3 + kPitchSemitones[1];
inline constexpr float kPitchB3 = kPitchA3 + kPitchSemitones[2];
inline constexpr float kPitchC3 = kPitchA3 + kPitchSemitones[3];
inline constexpr float kPitchCsharp3 = kPitchA3 + kPitchSemitones[4];
inline constexpr float kPitchD3 = kPitchA3 + kPitchSemitones[5];
inline constexpr float kPitchDsharp3 = kPitchA3 + kPitchSemitones[6];
inline constexpr float kPitchE3 = kPitchA3 + kPitchSemitones[7];
inline constexpr float kPitchF3 = kPitchA3 + kPitchSemitones[8];
inline constexpr float kPitchFsharp3 = kPitchA3 + kPitchSemitones[9];
inline constexpr float kPitchG3 = kPitchA3 + kPitchSemitones[10];
inline constexpr float kPitchGsharp3 = kPitchA3 + kPitchSemitones[11];
inline constexpr float kPitchA4 = 0.0f;
inline constexpr float kPitchAsharp4 = kPitchSemitones[1];
inline constexpr float kPitchB4 = kPitchSemitones[2];
inline constexpr float kPitchC4 = kPitchSemitones[3];
inline constexpr float kPitchCsharp4 = kPitchSemitones[4];
inline constexpr float kPitchD4 = kPitchSemitones[5];
inline constexpr float kPitchDsharp4 = kPitchSemitones[6];
inline constexpr float kPitchE4 = kPitchSemitones[7];
inline constexpr float kPitchF4 = kPitchSemitones[8];
inline constexpr float kPitchFsharp4 = kPitchSemitones[9];
inline constexpr float kPitchG4 = kPitchSemitones[10];
inline constexpr float kPitchGsharp4 = kPitchSemitones[11];
inline constexpr float kPitchA5 = 1.0f;
inline constexpr float kPitchAsharp5 = kPitchA5 + kPitchSemitones[1];
inline constexpr float kPitchB5 = kPitchA5 + kPitchSemitones[2];
inline constexpr float kPitchC5 = kPitchA5 + kPitchSemitones[3];
inline constexpr float kPitchCsharp5 = kPitchA5 + kPitchSemitones[4];
inline constexpr float kPitchD5 = kPitchA5 + kPitchSemitones[5];
inline constexpr float kPitchDsharp5 = kPitchA5 + kPitchSemitones[6];
inline constexpr float kPitchE5 = kPitchA5 + kPitchSemitones[7];
inline constexpr float kPitchF5 = kPitchA5 + kPitchSemitones[8];
inline constexpr float kPitchFsharp5 = kPitchA5 + kPitchSemitones[9];
inline constexpr float kPitchG5 = kPitchA5 + kPitchSemitones[10];
inline constexpr float kPitchGsharp5 = kPitchA5 + kPitchSemitones[11];
inline constexpr float kPitchA6 = 2.0f;
inline constexpr float kPitchAsharp6 = kPitchA6 + kPitchSemitones[1];
inline constexpr float kPitchB6 = kPitchA6 + kPitchSemitones[2];
inline constexpr float kPitchC6 = kPitchA6 + kPitchSemitones[3];
inline constexpr float kPitchCsharp6 = kPitchA6 + kPitchSemitones[4];
inline constexpr float kPitchD6 = kPitchA6 + kPitchSemitones[5];
inline constexpr float kPitchDsharp6 = kPitchA6 + kPitchSemitones[6];
inline constexpr float kPitchE6 = kPitchA6 + kPitchSemitones[7];
inline constexpr float kPitchF6 = kPitchA6 + kPitchSemitones[8];
inline constexpr float kPitchFsharp6 = kPitchA6 + kPitchSemitones[9];
inline constexpr float kPitchG6 = kPitchA6 + kPitchSemitones[10];
inline constexpr float kPitchGsharp6 = kPitchA6 + kPitchSemitones[11];
inline constexpr float kPitchA7 = 3.0f;
inline constexpr float kPitchAsharp7 = kPitchA7 + kPitchSemitones[1];
inline constexpr float kPitchB7 = kPitchA7 + kPitchSemitones[2];
inline constexpr float kPitchC7 = kPitchA7 + kPitchSemitones[3];
inline constexpr float kPitchCsharp7 = kPitchA7 + kPitchSemitones[4];
inline constexpr float kPitchD7 = kPitchA7 + kPitchSemitones[5];
inline constexpr float kPitchDsharp7 = kPitchA7 + kPitchSemitones[6];
inline constexpr float kPitchE7 = kPitchA7 + kPitchSemitones[7];
inline constexpr float kPitchF7 = kPitchA7 + kPitchSemitones[8];
inline constexpr float kPitchFsharp7 = kPitchA7 + kPitchSemitones[9];
inline constexpr float kPitchG7 = kPitchA7 + kPitchSemitones[10];
inline constexpr float kPitchGsharp7 = kPitchA7 + kPitchSemitones[11];
inline constexpr float kPitchKick = kPitchC3;
inline constexpr float kPitchSnare = kPitchD3;
inline constexpr float kPitchHihatClosed = kPitchE3;
inline constexpr float kPitchHihatOpen = kPitchF3;

/// Note pitch type.
using NotePitch = std::variant<float>;

/// Returns note pitch for a given scale and index.
///
/// @param scale Cumulative scale intervals of an octave in increasing order.
/// @param index Scale index.
/// @return Note pitch.
float GetPitch(std::span<const float> scale, int index);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_PITCH_H_
