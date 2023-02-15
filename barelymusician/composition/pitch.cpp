#include "barelymusician/composition/pitch.h"

#include <cmath>

namespace barely {

double PitchFromScale(std::span<const double> scale, int index) noexcept {
  if (!scale.empty()) {
    const double scale_length = static_cast<double>(scale.size());
    const double octave_offset =
        std::floor(static_cast<double>(index) / scale_length);
    const int scale_offset =
        index - static_cast<int>(octave_offset * scale_length);
    return octave_offset + scale[scale_offset];
  }
  return 0.0;
}

}  // namespace barely
