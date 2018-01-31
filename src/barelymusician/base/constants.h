#ifndef BARELYMUSICIAN_BASE_CONSTANTS_H
#define BARELYMUSICIAN_BASE_CONSTANTS_H

namespace barelyapi {

// PI (radians).
const double kPi = 3.14159265358979323846;

// Two PI (radians).
const float kTwoPi = static_cast<float>(2.0 * kPi);

// Converts minutes to seconds.
const float kSecondsFromMinutes = 60.0f;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_CONSTANTS_H
