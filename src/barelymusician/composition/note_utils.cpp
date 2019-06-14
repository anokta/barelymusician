#include "barelymusician/composition/note_utils.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

float GetQuantizedBeatDuration(int num_notes, int num_notes_per_beat) {
  DCHECK_GE(num_notes, 0);
  DCHECK_GT(num_notes_per_beat, 0);
  const float num_beats = static_cast<float>(num_notes / num_notes_per_beat);
  return num_beats + static_cast<float>(num_notes % num_notes_per_beat) /
                         static_cast<float>(num_notes_per_beat);
}

float GetQuantizedNoteIntex(const std::vector<float>& scale,
                            float scale_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  // TODO(#42): Properly suport fractional |note_index| values?
  return kNumSemitones * octave_offset +
         scale[static_cast<int>(std::floor(scale_offset))];
}

}  // namespace barelyapi
