#ifndef BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_SLICE_STATE_H_

#include <cstdint>

namespace barely {

struct SliceState {
  // Mono samples.
  const float* samples = nullptr;
  int32_t sample_count = 0;

  int32_t sample_rate = 0;

  float root_pitch = 0.0f;

  uint32_t next_slice_index = UINT32_MAX;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
