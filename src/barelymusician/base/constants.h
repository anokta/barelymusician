#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H
#define BARELYMUSICIAN_BASE_CONSTANTS_H

namespace barelyapi {

// PI (radians).
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.0f * kPi;

// Converts minutes to seconds.
constexpr float kSecondsFromMinutes = 60.0f;

// Minimum dB threshold. 
constexpr float kMinDecibels = -80.0f;

// Number of semitones in an octave (twelwe-tone equal temperament).
constexpr float kNumSemitones = 12.0f;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H
