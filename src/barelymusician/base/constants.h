#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H_
#define BARELYMUSICIAN_BASE_CONSTANTS_H_

namespace barelyapi {

// PI (radians).
static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kTwoPi = 2.0f * kPi;

// Converts minutes to seconds.
static constexpr float kSecondsFromMinutes = 60.0f;

// Minimum dB threshold.
static constexpr float kMinDecibels = -80.0f;

// Number of semitones in an octave (twelwe-tone equal temperament).
static constexpr float kNumSemitones = 12.0f;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H_
