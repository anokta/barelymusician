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
// TODO(#29): This is an arbitrary value, make sure this is *always* safe.
static const int kNumMaxMessageDataBytes = 16;

// Number of semitones in an octave (twelwe-tone equal temperament).
static const float kNumSemitones = 12.0f;

// Common note indices.
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
static const float kNoteIndexKick = kNoteIndexC3;
static const float kNoteIndexSnare = kNoteIndexD3;
static const float kNoteIndexHihatClosed = kNoteIndexE3;
static const float kNoteIndexHihatOpen = kNoteIndexF3;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H_
