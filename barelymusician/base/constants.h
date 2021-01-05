#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H_
#define BARELYMUSICIAN_BASE_CONSTANTS_H_

namespace barelyapi {

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
inline constexpr float kPitchA0 = 21.0f;
inline constexpr float kPitchAsharp0 = 22.0f;
inline constexpr float kPitchB0 = 23.0f;
inline constexpr float kPitchC0 = 24.0f;
inline constexpr float kPitchCsharp0 = 25.0f;
inline constexpr float kPitchD0 = 26.0f;
inline constexpr float kPitchDsharp0 = 27.0f;
inline constexpr float kPitchE0 = 28.0f;
inline constexpr float kPitchF0 = 29.0f;
inline constexpr float kPitchFsharp0 = 30.0f;
inline constexpr float kPitchG0 = 31.0f;
inline constexpr float kPitchGsharp0 = 32.0f;
inline constexpr float kPitchA1 = 33.0f;
inline constexpr float kPitchAsharp1 = 34.0f;
inline constexpr float kPitchB1 = 35.0f;
inline constexpr float kPitchC1 = 36.0f;
inline constexpr float kPitchCsharp1 = 37.0f;
inline constexpr float kPitchD1 = 38.0f;
inline constexpr float kPitchDsharp1 = 39.0f;
inline constexpr float kPitchE1 = 40.0f;
inline constexpr float kPitchF1 = 41.0f;
inline constexpr float kPitchFsharp1 = 42.0f;
inline constexpr float kPitchG1 = 43.0f;
inline constexpr float kPitchGsharp1 = 44.0f;
inline constexpr float kPitchA2 = 45.0f;
inline constexpr float kPitchAsharp2 = 46.0f;
inline constexpr float kPitchB2 = 47.0f;
inline constexpr float kPitchC2 = 48.0f;
inline constexpr float kPitchCsharp2 = 49.0f;
inline constexpr float kPitchD2 = 50.0f;
inline constexpr float kPitchDsharp2 = 51.0f;
inline constexpr float kPitchE2 = 52.0f;
inline constexpr float kPitchF2 = 53.0f;
inline constexpr float kPitchFsharp2 = 54.0f;
inline constexpr float kPitchG2 = 55.0f;
inline constexpr float kPitchGsharp2 = 56.0f;
inline constexpr float kPitchA3 = 57.0f;
inline constexpr float kPitchAsharp3 = 58.0f;
inline constexpr float kPitchB3 = 59.0f;
inline constexpr float kPitchC3 = 60.0f;
inline constexpr float kPitchCsharp3 = 61.0f;
inline constexpr float kPitchD3 = 62.0f;
inline constexpr float kPitchDsharp3 = 63.0f;
inline constexpr float kPitchE3 = 64.0f;
inline constexpr float kPitchF3 = 65.0f;
inline constexpr float kPitchFsharp3 = 66.0f;
inline constexpr float kPitchG3 = 67.0f;
inline constexpr float kPitchGsharp3 = 68.0f;
inline constexpr float kPitchA4 = 69.0f;
inline constexpr float kPitchAsharp4 = 70.0f;
inline constexpr float kPitchB4 = 71.0f;
inline constexpr float kPitchC4 = 72.0f;
inline constexpr float kPitchCsharp4 = 73.0f;
inline constexpr float kPitchD4 = 74.0f;
inline constexpr float kPitchDsharp4 = 75.0f;
inline constexpr float kPitchE4 = 76.0f;
inline constexpr float kPitchF4 = 77.0f;
inline constexpr float kPitchFsharp4 = 78.0f;
inline constexpr float kPitchG4 = 79.0f;
inline constexpr float kPitchGsharp4 = 80.0f;
inline constexpr float kPitchA5 = 81.0f;
inline constexpr float kPitchAsharp5 = 82.0f;
inline constexpr float kPitchB5 = 83.0f;
inline constexpr float kPitchC5 = 84.0f;
inline constexpr float kPitchCsharp5 = 85.0f;
inline constexpr float kPitchD5 = 86.0f;
inline constexpr float kPitchDsharp5 = 87.0f;
inline constexpr float kPitchE5 = 88.0f;
inline constexpr float kPitchF5 = 89.0f;
inline constexpr float kPitchFsharp5 = 90.0f;
inline constexpr float kPitchG5 = 91.0f;
inline constexpr float kPitchGsharp5 = 92.0f;
inline constexpr float kPitchA6 = 93.0f;
inline constexpr float kPitchAsharp6 = 94.0f;
inline constexpr float kPitchB6 = 95.0f;
inline constexpr float kPitchC6 = 96.0f;
inline constexpr float kPitchCsharp6 = 97.0f;
inline constexpr float kPitchD6 = 98.0f;
inline constexpr float kPitchDsharp6 = 99.0f;
inline constexpr float kPitchE6 = 100.0f;
inline constexpr float kPitchF6 = 101.0f;
inline constexpr float kPitchFsharp6 = 102.0f;
inline constexpr float kPitchG6 = 103.0f;
inline constexpr float kPitchGsharp6 = 104.0f;
inline constexpr float kPitchA7 = 105.0f;
inline constexpr float kPitchAsharp7 = 106.0f;
inline constexpr float kPitchB7 = 107.0f;
inline constexpr float kPitchC7 = 108.0f;
inline constexpr float kPitchCsharp7 = 109.0f;
inline constexpr float kPitchD7 = 110.0f;
inline constexpr float kPitchDsharp7 = 111.0f;
inline constexpr float kPitchE7 = 112.0f;
inline constexpr float kPitchF7 = 1113.0f;
inline constexpr float kPitchFsharp7 = 114.0f;
inline constexpr float kPitchG7 = 115.0f;
inline constexpr float kPitchGsharp7 = 116.0f;
inline constexpr float kPitchKick = kPitchC3;
inline constexpr float kPitchSnare = kPitchD3;
inline constexpr float kPitchHihatClosed = kPitchE3;
inline constexpr float kPitchHihatOpen = kPitchF3;

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
