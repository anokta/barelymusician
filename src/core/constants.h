#ifndef BARELYMUSICIAN_CORE_CONSTANTS_H_
#define BARELYMUSICIAN_CORE_CONSTANTS_H_

#include <cstdint>

namespace barely {

// Envelope epsilon error which is set to -60 decibels.
inline constexpr double kEnvelopeEpsilon = 1e-3;

// Invalid pool index.
inline constexpr uint32_t kInvalidIndex = UINT32_MAX;

// Maximum delay feedback to keep the output stable.
inline constexpr double kMaxDelayFeedback = 0.9995;

// Minimum filter frequency in hertz.
inline constexpr double kMinFilterFreq = 20.0;

// Reference frequency which is tuned to middle C in hertz.
inline constexpr double kReferenceFreq = 261.62555;

// Number of stereo channels.
inline constexpr int kStereoChannelCount = 2;

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONSTANTS_H_
