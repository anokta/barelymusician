#include "barelymusician/composition/scale.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Scale::Scale(const std::vector<float>& intervals) : intervals_(intervals) {
  DCHECK(!intervals_.empty());
}

int Scale::GetLength() const { return static_cast<int>(intervals_.size()); }

float Scale::GetNoteIndex(float scale_index) const {
  const float scale_length = static_cast<float>(intervals_.size());
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  // TODO(#42): Properly suport fractional |note_index| values?
  return kNumSemitones * octave_offset +
         intervals_[static_cast<int>(std::floor(scale_offset))];
}

}  // namespace barelyapi
