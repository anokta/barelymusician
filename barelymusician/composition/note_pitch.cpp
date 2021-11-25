#include "barelymusician/composition/note_pitch.h"

#include <cmath>
#include <span>

#include "barelymusician/common/logging.h"

namespace barely {

float GetPitch(std::span<const float> scale, int index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset =
      std::floor(static_cast<float>(index) / scale_length);
  const int scale_offset =
      index - static_cast<int>(octave_offset * scale_length);

  return octave_offset + scale[scale_offset];
}

}  // namespace barely
