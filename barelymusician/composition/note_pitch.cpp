#include "barelymusician/composition/note_pitch.h"

#include <cassert>
#include <cmath>
#include <span>

namespace barely {

float GetPitch(std::span<const float> scale, int index) noexcept {
  assert(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset =
      std::floor(static_cast<float>(index) / scale_length);
  const int scale_offset =
      index - static_cast<int>(octave_offset * scale_length);

  return octave_offset + scale[scale_offset];
}

}  // namespace barely
