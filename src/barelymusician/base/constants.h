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

// Number of semitones in an octave (twelwe-tone equal temperament).
static const float kNumSemitones = 12.0f;

// Maximum number of bytes that the message data can contain.
// TODO(#29): This is an arbitrary value, make sure this is *always* safe.
static const int kNumMaxMessageDataBytes = 16;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H_
