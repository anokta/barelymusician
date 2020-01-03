#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H_
#define BARELYMUSICIAN_BASE_CONSTANTS_H_

namespace barelyapi {

// PI (radians).
static const float kPi = 3.14159265358979323846f;
static const float kTwoPi = 2.0f * kPi;

// Converts minutes to seconds.
static const float kSecondsFromMinutes = 60.0f;

// Minimum dB threshold.
static const float kMinDecibels = -80.0f;

// Number of mono channels.
static const int kNumMonoChannels = 1;

// Number of stereo channels.
static const int kNumStereoChannels = 2;

// Maximum number of bytes that the message data can contain.
static const int kNumMaxMessageDataBytes = 2 * sizeof(float);

// Number of semitones in an octave (twelwe-tone equal temperament).
static const float kNumSemitones = 12.0f;

// Common musical scales.
static const float kMajorScale[] = {0.0f, 2.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f};
static const float kNaturalMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f,
                                           7.0f, 8.0f, 10.0f};
static const float kHarmonicMinorScale[] = {0.0f, 2.0f, 3.0f, 5.0f,
                                            7.0f, 8.0f, 11.0f};

// Common note indices.
static const float kNoteIndexA0 = 21.0f;
static const float kNoteIndexAsharp0 = 22.0f;
static const float kNoteIndexB0 = 23.0f;
static const float kNoteIndexC0 = 24.0f;
static const float kNoteIndexCsharp0 = 25.0f;
static const float kNoteIndexD0 = 26.0f;
static const float kNoteIndexDsharp0 = 27.0f;
static const float kNoteIndexE0 = 28.0f;
static const float kNoteIndexF0 = 29.0f;
static const float kNoteIndexFsharp0 = 30.0f;
static const float kNoteIndexG0 = 31.0f;
static const float kNoteIndexGsharp0 = 32.0f;
static const float kNoteIndexA1 = 33.0f;
static const float kNoteIndexAsharp1 = 34.0f;
static const float kNoteIndexB1 = 35.0f;
static const float kNoteIndexC1 = 36.0f;
static const float kNoteIndexCsharp1 = 37.0f;
static const float kNoteIndexD1 = 38.0f;
static const float kNoteIndexDsharp1 = 39.0f;
static const float kNoteIndexE1 = 40.0f;
static const float kNoteIndexF1 = 41.0f;
static const float kNoteIndexFsharp1 = 42.0f;
static const float kNoteIndexG1 = 43.0f;
static const float kNoteIndexGsharp1 = 44.0f;
static const float kNoteIndexA2 = 45.0f;
static const float kNoteIndexAsharp2 = 46.0f;
static const float kNoteIndexB2 = 47.0f;
static const float kNoteIndexC2 = 48.0f;
static const float kNoteIndexCsharp2 = 49.0f;
static const float kNoteIndexD2 = 50.0f;
static const float kNoteIndexDsharp2 = 51.0f;
static const float kNoteIndexE2 = 52.0f;
static const float kNoteIndexF2 = 53.0f;
static const float kNoteIndexFsharp2 = 54.0f;
static const float kNoteIndexG2 = 55.0f;
static const float kNoteIndexGsharp2 = 56.0f;
static const float kNoteIndexA3 = 57.0f;
static const float kNoteIndexAsharp3 = 58.0f;
static const float kNoteIndexB3 = 59.0f;
static const float kNoteIndexC3 = 60.0f;
static const float kNoteIndexCsharp3 = 61.0f;
static const float kNoteIndexD3 = 62.0f;
static const float kNoteIndexDsharp3 = 63.0f;
static const float kNoteIndexE3 = 64.0f;
static const float kNoteIndexF3 = 65.0f;
static const float kNoteIndexFsharp3 = 66.0f;
static const float kNoteIndexG3 = 67.0f;
static const float kNoteIndexGsharp3 = 68.0f;
static const float kNoteIndexA4 = 69.0f;
static const float kNoteIndexAsharp4 = 70.0f;
static const float kNoteIndexB4 = 71.0f;
static const float kNoteIndexC4 = 72.0f;
static const float kNoteIndexCsharp4 = 73.0f;
static const float kNoteIndexD4 = 74.0f;
static const float kNoteIndexDsharp4 = 75.0f;
static const float kNoteIndexE4 = 76.0f;
static const float kNoteIndexF4 = 77.0f;
static const float kNoteIndexFsharp4 = 78.0f;
static const float kNoteIndexG4 = 79.0f;
static const float kNoteIndexGsharp4 = 80.0f;
static const float kNoteIndexA5 = 81.0f;
static const float kNoteIndexAsharp5 = 82.0f;
static const float kNoteIndexB5 = 83.0f;
static const float kNoteIndexC5 = 84.0f;
static const float kNoteIndexCsharp5 = 85.0f;
static const float kNoteIndexD5 = 86.0f;
static const float kNoteIndexDsharp5 = 87.0f;
static const float kNoteIndexE5 = 88.0f;
static const float kNoteIndexF5 = 89.0f;
static const float kNoteIndexFsharp5 = 90.0f;
static const float kNoteIndexG5 = 91.0f;
static const float kNoteIndexGsharp5 = 92.0f;
static const float kNoteIndexA6 = 93.0f;
static const float kNoteIndexAsharp6 = 94.0f;
static const float kNoteIndexB6 = 95.0f;
static const float kNoteIndexC6 = 96.0f;
static const float kNoteIndexCsharp6 = 97.0f;
static const float kNoteIndexD6 = 98.0f;
static const float kNoteIndexDsharp6 = 99.0f;
static const float kNoteIndexE6 = 100.0f;
static const float kNoteIndexF6 = 101.0f;
static const float kNoteIndexFsharp6 = 102.0f;
static const float kNoteIndexG6 = 103.0f;
static const float kNoteIndexGsharp6 = 104.0f;
static const float kNoteIndexA7 = 105.0f;
static const float kNoteIndexAsharp7 = 106.0f;
static const float kNoteIndexB7 = 107.0f;
static const float kNoteIndexC7 = 108.0f;
static const float kNoteIndexCsharp7 = 109.0f;
static const float kNoteIndexD7 = 110.0f;
static const float kNoteIndexDsharp7 = 111.0f;
static const float kNoteIndexE7 = 112.0f;
static const float kNoteIndexF7 = 1113.0f;
static const float kNoteIndexFsharp7 = 114.0f;
static const float kNoteIndexG7 = 115.0f;
static const float kNoteIndexGsharp7 = 116.0f;
static const float kNoteIndexKick = kNoteIndexC3;
static const float kNoteIndexSnare = kNoteIndexD3;
static const float kNoteIndexHihatClosed = kNoteIndexE3;
static const float kNoteIndexHihatOpen = kNoteIndexF3;

// Common note values in relation to quarter note beat duration.
static const int kNumQuarterNotesPerBeat = 1;
static const int kNumEighthNotesPerBeat = 2;
static const int kNumEighthTripletNotesPerBeat = 3;
static const int kNumSixteenthNotesPerBeat = 4;
static const int kNumSixteenthTripletNotesPerBeat = 6;
static const int kNumThirtySecondNotesPerBeat = 8;
static const int kNumThirtySecondTripletNotesPerBeat = 12;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H_
