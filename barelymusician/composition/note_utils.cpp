#include "barelymusician/composition/note_utils.h"

#include <cmath>

#include "barelymusician/common/logging.h"

namespace barelyapi {

namespace {

// Linearly interpolates between given points.
double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

int GetNoteNumber(const std::vector<int>& scale, int scale_index) {
  DCHECK(!scale.empty());
  const int scale_length = static_cast<int>(scale.size());
  const int octave_offset = static_cast<int>(std::floor(
      static_cast<float>(scale_index) / static_cast<float>(scale_length)));
  const int scale_offset = scale_index - octave_offset * scale_length;
  return kNumSemitones * octave_offset + scale[scale_offset];
}

float GetPitch(int note_number) {
  return static_cast<float>(note_number - kNoteNumberA4) /
         static_cast<float>(kNumSemitones);
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
