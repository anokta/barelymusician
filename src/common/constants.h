#ifndef BARELYMUSICIAN_COMMON_CONSTANTS_H_
#define BARELYMUSICIAN_COMMON_CONSTANTS_H_

#include <cstdint>

namespace barely {

/// Reference frequency which is tuned to middle C in hertz.
inline constexpr float kReferenceFrequency = 261.62555f;

/// Number of stereo channels.
inline constexpr int kStereoChannelCount = 2;

/// Maximum number of instruments.
inline constexpr uint32_t kMaxInstrumentCount = BARELYMUSICIAN_MAX_INSTRUMENT_COUNT;

/// Maximum number of performers.
inline constexpr uint32_t kMaxPerformerCount = BARELYMUSICIAN_MAX_PERFORMER_COUNT;

/// Maximum number of tasks.
inline constexpr uint32_t kMaxTaskCount = BARELYMUSICIAN_MAX_TASK_COUNT;

/// Maximum number of active voices.
inline constexpr int kMaxActiveVoiceCount = BARELYMUSICIAN_MAX_VOICE_COUNT;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_CONSTANTS_H_
