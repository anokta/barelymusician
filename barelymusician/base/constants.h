#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H_
#define BARELYMUSICIAN_BASE_CONSTANTS_H_

#include "barelymusician/base/types.h"

namespace barelyapi {

// Invalid id.
inline constexpr Id kInvalidId = -1;

// Converts minutes to seconds.
inline constexpr double kSecondsFromMinutes = 60.0;

// PI (radians).
inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = 2.0f * kPi;

// Minimum dB threshold.
inline constexpr float kMinDecibels = -80.0f;

// Number of mono channels.
inline constexpr int kNumMonoChannels = 1;

// Number of stereo channels.
inline constexpr int kNumStereoChannels = 2;

// Number of semitones in an octave (twelwe-tone equal temperament).
inline constexpr float kNumSemitones = 12.0f;

// Common musical scales.
inline constexpr float kMajorScale[] = {0.0f, 2.0f, 4.0f, 5.0f,
                                        7.0f, 9.0f, 11.0f};
inline constexpr float kNaturalMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f,
                                               7.0f, 8.0f, 10.0f};
inline constexpr float kHarmonicMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f,
                                                7.0f, 8.0f, 11.0f};

// Middle A note (A4) frequency.
inline constexpr float kFrequencyA4 = 440.0f;

// Common note indices.
inline constexpr float kNoteIndexA0 = 21.0f;
inline constexpr float kNoteIndexAsharp0 = 22.0f;
inline constexpr float kNoteIndexB0 = 23.0f;
inline constexpr float kNoteIndexC0 = 24.0f;
inline constexpr float kNoteIndexCsharp0 = 25.0f;
inline constexpr float kNoteIndexD0 = 26.0f;
inline constexpr float kNoteIndexDsharp0 = 27.0f;
inline constexpr float kNoteIndexE0 = 28.0f;
inline constexpr float kNoteIndexF0 = 29.0f;
inline constexpr float kNoteIndexFsharp0 = 30.0f;
inline constexpr float kNoteIndexG0 = 31.0f;
inline constexpr float kNoteIndexGsharp0 = 32.0f;
inline constexpr float kNoteIndexA1 = 33.0f;
inline constexpr float kNoteIndexAsharp1 = 34.0f;
inline constexpr float kNoteIndexB1 = 35.0f;
inline constexpr float kNoteIndexC1 = 36.0f;
inline constexpr float kNoteIndexCsharp1 = 37.0f;
inline constexpr float kNoteIndexD1 = 38.0f;
inline constexpr float kNoteIndexDsharp1 = 39.0f;
inline constexpr float kNoteIndexE1 = 40.0f;
inline constexpr float kNoteIndexF1 = 41.0f;
inline constexpr float kNoteIndexFsharp1 = 42.0f;
inline constexpr float kNoteIndexG1 = 43.0f;
inline constexpr float kNoteIndexGsharp1 = 44.0f;
inline constexpr float kNoteIndexA2 = 45.0f;
inline constexpr float kNoteIndexAsharp2 = 46.0f;
inline constexpr float kNoteIndexB2 = 47.0f;
inline constexpr float kNoteIndexC2 = 48.0f;
inline constexpr float kNoteIndexCsharp2 = 49.0f;
inline constexpr float kNoteIndexD2 = 50.0f;
inline constexpr float kNoteIndexDsharp2 = 51.0f;
inline constexpr float kNoteIndexE2 = 52.0f;
inline constexpr float kNoteIndexF2 = 53.0f;
inline constexpr float kNoteIndexFsharp2 = 54.0f;
inline constexpr float kNoteIndexG2 = 55.0f;
inline constexpr float kNoteIndexGsharp2 = 56.0f;
inline constexpr float kNoteIndexA3 = 57.0f;
inline constexpr float kNoteIndexAsharp3 = 58.0f;
inline constexpr float kNoteIndexB3 = 59.0f;
inline constexpr float kNoteIndexC3 = 60.0f;
inline constexpr float kNoteIndexCsharp3 = 61.0f;
inline constexpr float kNoteIndexD3 = 62.0f;
inline constexpr float kNoteIndexDsharp3 = 63.0f;
inline constexpr float kNoteIndexE3 = 64.0f;
inline constexpr float kNoteIndexF3 = 65.0f;
inline constexpr float kNoteIndexFsharp3 = 66.0f;
inline constexpr float kNoteIndexG3 = 67.0f;
inline constexpr float kNoteIndexGsharp3 = 68.0f;
inline constexpr float kNoteIndexA4 = 69.0f;
inline constexpr float kNoteIndexAsharp4 = 70.0f;
inline constexpr float kNoteIndexB4 = 71.0f;
inline constexpr float kNoteIndexC4 = 72.0f;
inline constexpr float kNoteIndexCsharp4 = 73.0f;
inline constexpr float kNoteIndexD4 = 74.0f;
inline constexpr float kNoteIndexDsharp4 = 75.0f;
inline constexpr float kNoteIndexE4 = 76.0f;
inline constexpr float kNoteIndexF4 = 77.0f;
inline constexpr float kNoteIndexFsharp4 = 78.0f;
inline constexpr float kNoteIndexG4 = 79.0f;
inline constexpr float kNoteIndexGsharp4 = 80.0f;
inline constexpr float kNoteIndexA5 = 81.0f;
inline constexpr float kNoteIndexAsharp5 = 82.0f;
inline constexpr float kNoteIndexB5 = 83.0f;
inline constexpr float kNoteIndexC5 = 84.0f;
inline constexpr float kNoteIndexCsharp5 = 85.0f;
inline constexpr float kNoteIndexD5 = 86.0f;
inline constexpr float kNoteIndexDsharp5 = 87.0f;
inline constexpr float kNoteIndexE5 = 88.0f;
inline constexpr float kNoteIndexF5 = 89.0f;
inline constexpr float kNoteIndexFsharp5 = 90.0f;
inline constexpr float kNoteIndexG5 = 91.0f;
inline constexpr float kNoteIndexGsharp5 = 92.0f;
inline constexpr float kNoteIndexA6 = 93.0f;
inline constexpr float kNoteIndexAsharp6 = 94.0f;
inline constexpr float kNoteIndexB6 = 95.0f;
inline constexpr float kNoteIndexC6 = 96.0f;
inline constexpr float kNoteIndexCsharp6 = 97.0f;
inline constexpr float kNoteIndexD6 = 98.0f;
inline constexpr float kNoteIndexDsharp6 = 99.0f;
inline constexpr float kNoteIndexE6 = 100.0f;
inline constexpr float kNoteIndexF6 = 101.0f;
inline constexpr float kNoteIndexFsharp6 = 102.0f;
inline constexpr float kNoteIndexG6 = 103.0f;
inline constexpr float kNoteIndexGsharp6 = 104.0f;
inline constexpr float kNoteIndexA7 = 105.0f;
inline constexpr float kNoteIndexAsharp7 = 106.0f;
inline constexpr float kNoteIndexB7 = 107.0f;
inline constexpr float kNoteIndexC7 = 108.0f;
inline constexpr float kNoteIndexCsharp7 = 109.0f;
inline constexpr float kNoteIndexD7 = 110.0f;
inline constexpr float kNoteIndexDsharp7 = 111.0f;
inline constexpr float kNoteIndexE7 = 112.0f;
inline constexpr float kNoteIndexF7 = 1113.0f;
inline constexpr float kNoteIndexFsharp7 = 114.0f;
inline constexpr float kNoteIndexG7 = 115.0f;
inline constexpr float kNoteIndexGsharp7 = 116.0f;
inline constexpr float kNoteIndexKick = kNoteIndexC3;
inline constexpr float kNoteIndexSnare = kNoteIndexD3;
inline constexpr float kNoteIndexHihatClosed = kNoteIndexE3;
inline constexpr float kNoteIndexHihatOpen = kNoteIndexF3;

// Common note values in relation to quarter note beat duration.
inline constexpr int kNumQuarterNotesPerBeat = 1;
inline constexpr int kNumEighthNotesPerBeat = 2;
inline constexpr int kNumEighthTripletNotesPerBeat = 3;
inline constexpr int kNumSixteenthNotesPerBeat = 4;
inline constexpr int kNumSixteenthTripletNotesPerBeat = 6;
inline constexpr int kNumThirtySecondNotesPerBeat = 8;
inline constexpr int kNumThirtySecondTripletNotesPerBeat = 12;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H_
