#ifndef BARELYMUSICIAN_CORE_SCALE_H_
#define BARELYMUSICIAN_CORE_SCALE_H_

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

namespace barely {

[[nodiscard]] inline double GetPitch(const BarelyScale& scale, int32_t degree) noexcept {
  if (scale.pitches == nullptr || scale.pitch_count <= 0) return 0.0;
  const int32_t scale_degree = degree + std::clamp<int32_t>(scale.mode, 0, scale.pitch_count);
  const int octave = static_cast<int>(
      std::floor(static_cast<double>(scale_degree) / static_cast<double>(scale.pitch_count)));
  const int32_t index = scale_degree - octave * scale.pitch_count;
  assert(index >= 0 && index < scale.pitch_count);
  return scale.root_pitch + static_cast<double>(octave) + scale.pitches[index] -
         scale.pitches[scale.mode];
}

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_SCALE_H_
