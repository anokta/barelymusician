#ifndef BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
#define BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_

#include <cassert>
#include <cmath>

#include "barelymusician.h"

namespace barely::internal {

template <SamplePlaybackMode kMode>
double PlaySample(const SampleDataSlice& slice, double increment, double& cursor) {
  if constexpr (kMode != SamplePlaybackMode::kNone) {
    assert(cursor >= 0.0);
    assert(increment >= 0.0);
    if (static_cast<int>(cursor) >= slice.sample_count) {
      return 0.0;
    }
    // TODO(#7): Add a better interpolation method here?
    const double output = slice.samples[static_cast<int>(cursor)];
    cursor += increment;
    if constexpr (kMode == SamplePlaybackMode::kLoop) {
      if (static_cast<int>(cursor) >= slice.sample_count) {
        cursor = std::fmod(cursor, static_cast<double>(slice.sample_count));
      }
    }
    return output;
  } else {
    return 0.0;
  }
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_SAMPLE_PLAYER_H_
