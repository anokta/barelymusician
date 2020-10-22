#include "barelymusician/engine/note_utils.h"

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

float GetRawNoteIndex(const std::vector<float>& scale,
                      const QuantizedNoteIndex& note_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float scale_index = static_cast<float>(note_index.scale_index);
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  return kNumSemitones * octave_offset +
         scale[static_cast<int>(std::floor(scale_offset))];
}

double GetRawPosition(const QuantizedPosition& position) {
  DCHECK_GE(position.step, 0);
  DCHECK_GT(position.num_steps, 0);
  const double num_beats =
      static_cast<double>(position.step / position.num_steps);
  return num_beats + static_cast<double>(position.step % position.num_steps) /
                         static_cast<double>(position.num_steps);
}

}  // namespace barelyapi
