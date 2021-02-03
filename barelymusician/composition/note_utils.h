#ifndef BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
#define BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_

#include <vector>

#include "barelymusician/composition/note.h"

namespace barelyapi {

/// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr int kNumSemitones = 12;

/// Common musical scales.
inline constexpr int kMajorScale[] = {0, 2, 4, 5, 7, 9, 11};
inline constexpr int kNaturalMinorScale[] = {0, 2, 3, 5, 7, 8, 10};
inline constexpr int kHarmonicMinorScale[] = {0, 2, 3, 5, 7, 8, 11};

/// Common note numbers.
inline constexpr int kNoteNumberA0 = 21;
inline constexpr int kNoteNumberAsharp0 = 22;
inline constexpr int kNoteNumberB0 = 23;
inline constexpr int kNoteNumberC0 = 24;
inline constexpr int kNoteNumberCsharp0 = 25;
inline constexpr int kNoteNumberD0 = 26;
inline constexpr int kNoteNumberDsharp0 = 27;
inline constexpr int kNoteNumberE0 = 28;
inline constexpr int kNoteNumberF0 = 29;
inline constexpr int kNoteNumberFsharp0 = 30;
inline constexpr int kNoteNumberG0 = 31;
inline constexpr int kNoteNumberGsharp0 = 32;
inline constexpr int kNoteNumberA1 = 33;
inline constexpr int kNoteNumberAsharp1 = 34;
inline constexpr int kNoteNumberB1 = 35;
inline constexpr int kNoteNumberC1 = 36;
inline constexpr int kNoteNumberCsharp1 = 37;
inline constexpr int kNoteNumberD1 = 38;
inline constexpr int kNoteNumberDsharp1 = 39;
inline constexpr int kNoteNumberE1 = 40;
inline constexpr int kNoteNumberF1 = 41;
inline constexpr int kNoteNumberFsharp1 = 42;
inline constexpr int kNoteNumberG1 = 43;
inline constexpr int kNoteNumberGsharp1 = 44;
inline constexpr int kNoteNumberA2 = 45;
inline constexpr int kNoteNumberAsharp2 = 46;
inline constexpr int kNoteNumberB2 = 47;
inline constexpr int kNoteNumberC2 = 48;
inline constexpr int kNoteNumberCsharp2 = 49;
inline constexpr int kNoteNumberD2 = 50;
inline constexpr int kNoteNumberDsharp2 = 51;
inline constexpr int kNoteNumberE2 = 52;
inline constexpr int kNoteNumberF2 = 53;
inline constexpr int kNoteNumberFsharp2 = 54;
inline constexpr int kNoteNumberG2 = 55;
inline constexpr int kNoteNumberGsharp2 = 56;
inline constexpr int kNoteNumberA3 = 57;
inline constexpr int kNoteNumberAsharp3 = 58;
inline constexpr int kNoteNumberB3 = 59;
inline constexpr int kNoteNumberC3 = 60;
inline constexpr int kNoteNumberCsharp3 = 61;
inline constexpr int kNoteNumberD3 = 62;
inline constexpr int kNoteNumberDsharp3 = 63;
inline constexpr int kNoteNumberE3 = 64;
inline constexpr int kNoteNumberF3 = 65;
inline constexpr int kNoteNumberFsharp3 = 66;
inline constexpr int kNoteNumberG3 = 67;
inline constexpr int kNoteNumberGsharp3 = 68;
inline constexpr int kNoteNumberA4 = 69;
inline constexpr int kNoteNumberAsharp4 = 70;
inline constexpr int kNoteNumberB4 = 71;
inline constexpr int kNoteNumberC4 = 72;
inline constexpr int kNoteNumberCsharp4 = 73;
inline constexpr int kNoteNumberD4 = 74;
inline constexpr int kNoteNumberDsharp4 = 75;
inline constexpr int kNoteNumberE4 = 76;
inline constexpr int kNoteNumberF4 = 77;
inline constexpr int kNoteNumberFsharp4 = 78;
inline constexpr int kNoteNumberG4 = 79;
inline constexpr int kNoteNumberGsharp4 = 80;
inline constexpr int kNoteNumberA5 = 81;
inline constexpr int kNoteNumberAsharp5 = 82;
inline constexpr int kNoteNumberB5 = 83;
inline constexpr int kNoteNumberC5 = 84;
inline constexpr int kNoteNumberCsharp5 = 85;
inline constexpr int kNoteNumberD5 = 86;
inline constexpr int kNoteNumberDsharp5 = 87;
inline constexpr int kNoteNumberE5 = 88;
inline constexpr int kNoteNumberF5 = 89;
inline constexpr int kNoteNumberFsharp5 = 90;
inline constexpr int kNoteNumberG5 = 91;
inline constexpr int kNoteNumberGsharp5 = 92;
inline constexpr int kNoteNumberA6 = 93;
inline constexpr int kNoteNumberAsharp6 = 94;
inline constexpr int kNoteNumberB6 = 95;
inline constexpr int kNoteNumberC6 = 96;
inline constexpr int kNoteNumberCsharp6 = 97;
inline constexpr int kNoteNumberD6 = 98;
inline constexpr int kNoteNumberDsharp6 = 99;
inline constexpr int kNoteNumberE6 = 100;
inline constexpr int kNoteNumberF6 = 101;
inline constexpr int kNoteNumberFsharp6 = 102;
inline constexpr int kNoteNumberG6 = 103;
inline constexpr int kNoteNumberGsharp6 = 104;
inline constexpr int kNoteNumberA7 = 105;
inline constexpr int kNoteNumberAsharp7 = 106;
inline constexpr int kNoteNumberB7 = 107;
inline constexpr int kNoteNumberC7 = 108;
inline constexpr int kNoteNumberCsharp7 = 109;
inline constexpr int kNoteNumberD7 = 110;
inline constexpr int kNoteNumberDsharp7 = 111;
inline constexpr int kNoteNumberE7 = 112;
inline constexpr int kNoteNumberF7 = 1113;
inline constexpr int kNoteNumberFsharp7 = 114;
inline constexpr int kNoteNumberG7 = 115;
inline constexpr int kNoteNumberGsharp7 = 116;
inline constexpr int kNoteNumberKick = kNoteNumberC3;
inline constexpr int kNoteNumberSnare = kNoteNumberD3;
inline constexpr int kNoteNumberHihatClosed = kNoteNumberE3;
inline constexpr int kNoteNumberHihatOpen = kNoteNumberF3;

/// Common note values in relation to quarter note beat duration.
inline constexpr int kNumQuarterNotesPerBeat = 1;
inline constexpr int kNumEighthNotesPerBeat = 2;
inline constexpr int kNumEighthTripletNotesPerBeat = 3;
inline constexpr int kNumSixteenthNotesPerBeat = 4;
inline constexpr int kNumSixteenthTripletNotesPerBeat = 6;
inline constexpr int kNumThirtySecondNotesPerBeat = 8;
inline constexpr int kNumThirtySecondTripletNotesPerBeat = 12;

/// Returns note number for the given scale and scale index.
///
/// @param scale Cumulative scale intervals of an octave in increasing order.
/// @param scale_index Scale index.
/// @return Note number in semitones.
int GetNoteNumber(const std::vector<int>& scale, int scale_index);

/// Returns pitch for the given note number.
///
/// @param note_number Note number in semitones.
/// @return Pitch.
float GetPitch(int note_number);

/// Returns quantized position for the given number of beat steps.
///
/// @param step Quantized step.
/// @param num_steps Number of steps per beat.
/// @return Raw position.
double GetPosition(int step, int num_steps);

/// Returns quantized position.
///
/// @param position Original position.
/// @param resolution Quantization resolution.
/// @param amount Quantization amount.
/// @return Quantized position.
double QuantizePosition(double position, double resolution,
                        double amount = 1.0);

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_NOTE_UTILS_H_
