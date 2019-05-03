#include "barelymusician/composition/note_utils.h"

#include <cmath>

#include "barelymusician//base/constants.h"

namespace barelyapi {

float GetScaledNoteIndex(float note_index, const std::vector<float>& scale) {
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset = std::floor(note_index / scale_length);
  const float scale_offset = note_index - octave_offset * scale_length;
  // TODO(#42): Properly suport fractional |note_index| values?
  const int scale_index = static_cast<int>(std::floor(scale_offset));
  return scale[scale_index] + octave_offset * kNumSemitones;
}

}  // namespace barelyapi
