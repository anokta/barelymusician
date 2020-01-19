#include "barelymusician/musician/note_utils.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

bool CompareNote(const Note& lhs, const Note& rhs) {
  return lhs.offset_beats < rhs.offset_beats;
}

bool CompareOffsetBeats(const Note& note, double offset_beats) {
  return note.offset_beats < offset_beats;
}

double GetBeat(int step, int num_steps_per_beat) {
  DCHECK_GE(step, 0);
  DCHECK_GT(num_steps_per_beat, 0);
  const double num_beats = static_cast<double>(step / num_steps_per_beat);
  return num_beats + static_cast<double>(step % num_steps_per_beat) /
                         static_cast<double>(num_steps_per_beat);
}

float GetNoteIndex(const std::vector<float>& scale, float scale_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  // TODO(#42): Properly suport fractional |note_index| values?
  return kNumSemitones * octave_offset +
         scale[static_cast<int>(std::floor(scale_offset))];
}

}  // namespace barelyapi
