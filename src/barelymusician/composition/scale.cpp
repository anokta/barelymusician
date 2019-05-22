#include "barelymusician/composition/scale.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

Scale::Scale(const std::vector<float>& intervals) : intervals_(intervals) {
  DCHECK(!intervals_.empty());
}

int Scale::GetLength() const { return static_cast<int>(intervals_.size()); }

float Scale::GetNoteIndex(float relative_note_index) const {
  const float scale_length = static_cast<float>(intervals_.size());
  const float octave_offset = std::floor(relative_note_index / scale_length);
  const float scale_offset = relative_note_index - octave_offset * scale_length;
  // TODO(#42): Properly suport fractional |note_index| values?
  const int scale_index = static_cast<int>(std::floor(scale_offset));
  return kNumSemitones * octave_offset + intervals_[scale_index];
}

}  // namespace barelyapi
