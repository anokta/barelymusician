#ifndef BARELYMUSICIAN_CORE_CONSTANTS_H_
#define BARELYMUSICIAN_CORE_CONSTANTS_H_

#include <cstdint>

namespace barely {

/// Reference frequency which is tuned to middle C in hertz.
inline constexpr float kReferenceFrequency = 261.62555f;

/// Number of stereo channels.
inline constexpr int kStereoChannelCount = 2;

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONSTANTS_H_
