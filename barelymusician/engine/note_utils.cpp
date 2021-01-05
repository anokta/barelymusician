#include "barelymusician/engine/note_utils.h"

#include <cmath>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/logging.h"

namespace barelyapi {

namespace {

double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

float GetPitch(const std::vector<float>& scale,
               const QuantizedNoteIndex& note_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float scale_index = static_cast<float>(note_index.scale_index);
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  return note_index.root_index + kNumSemitones * octave_offset +
         scale[static_cast<int>(std::floor(scale_offset))];
}

double GetPosition(int step, int num_steps) {
  DCHECK_GE(step, 0);
  DCHECK_GT(num_steps, 0);
  const double num_beats = static_cast<double>(step / num_steps);
  return num_beats +
         static_cast<double>(step % num_steps) / static_cast<double>(num_steps);
}

double QuantizePosition(double position, double resolution, double amount) {
  DCHECK_GT(resolution, 0.0);
  DCHECK_GE(amount, 0.0);
  DCHECK_LE(amount, 1.0);
  return Lerp(position, resolution * std::round(position / resolution), amount);
}

}  // namespace barelyapi
