#ifndef BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_SLICE_STATE_H_

#include <cstdint>

namespace barely {

struct SliceState {
  /// Array of mono samples.
  const float* samples = nullptr;

  /// Number of mono samples.
  int32_t sample_count = 0;

  /// Sampling rate in hertz.
  int32_t sample_rate = 0;

  /// Root note pitch.
  float root_pitch = 0.0f;

  /// Next slice index.
  uint32_t next_slice_index = UINT32_MAX;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
