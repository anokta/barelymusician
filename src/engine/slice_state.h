#ifndef BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
#define BARELYMUSICIAN_ENGINE_SLICE_STATE_H_

#include <cstdint>

#include "core/constants.h"

namespace barely {

struct SliceState {
  double root_pitch = 0.0;

  double sample_rate = 0.0;

  const double* samples = nullptr;  // mono
  int32_t sample_count = 0;

  uint32_t next_slice_index = kInvalidIndex;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_ENGINE_SLICE_STATE_H_
