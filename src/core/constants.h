#ifndef BARELYMUSICIAN_CORE_CONSTANTS_H_
#define BARELYMUSICIAN_CORE_CONSTANTS_H_

#include <cstdint>

namespace barely {

// Invalid pool index.
inline constexpr uint32_t kInvalidIndex = UINT32_MAX;

// Maximum delay feedback to keep the output stable.
inline constexpr float kMaxDelayFeedback = 0.9995f;

// Maximum number of delay frames.
inline constexpr int kMaxDelayFrameCount = 8 * 48000;

// Minimum filter frequency in hertz.
static constexpr float kMinFilterFreq = 20.0f;

// Reference frequency which is tuned to middle C in hertz.
inline constexpr float kReferenceFreq = 261.62555f;

// Number of stereo channels.
inline constexpr int kStereoChannelCount = 2;

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_CONSTANTS_H_
