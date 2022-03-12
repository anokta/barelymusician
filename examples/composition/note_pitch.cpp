#include "examples/composition/note_pitch.h"

#include <cassert>
#include <cmath>

namespace barelyapi {

double GetPitch(const std::vector<double>& scale, int index) noexcept {
  assert(!scale.empty());
  const double scale_length = static_cast<double>(scale.size());
  const double octave_offset =
      std::floor(static_cast<double>(index) / scale_length);
  const int scale_offset =
      index - static_cast<int>(octave_offset * scale_length);

  return octave_offset + scale[scale_offset];
}

}  // namespace barelyapi
