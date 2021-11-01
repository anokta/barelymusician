#include "barelymusician/composition/note_pitch.h"

#include <cmath>
#include <vector>

#include "barelymusician/common/logging.h"

namespace barelyapi {

float GetPitch(const std::vector<float>& scale, int scale_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset =
      std::floor(static_cast<float>(scale_index) / scale_length);
  const int scale_offset =
      scale_index - static_cast<int>(octave_offset * scale_length);

  return octave_offset + scale[scale_offset];
}

}  // namespace barelyapi
