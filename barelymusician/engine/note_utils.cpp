#include "barelymusician/engine/note_utils.h"

#include <cmath>

#include "barelymusician/common/logging.h"

namespace barelyapi {

namespace {

// Middle A (A4) frequency.
constexpr float kFrequencyA4 = 440.0f;

// Linearly interpolates between given points.
double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

float GetFrequency(float pitch) {
  // Middle A note (A4) is selected as the base note frequency, where:
  //  f = fA4 * 2 ^ ((i - pA4) / 12).
  return kFrequencyA4 * std::pow(2.0f, (pitch - kPitchA4) / kNumSemitones);
}

float GetPitch(const std::vector<float>& scale, float scale_index) {
  DCHECK(!scale.empty());
  const float scale_length = static_cast<float>(scale.size());
  const float octave_offset = std::floor(scale_index / scale_length);
  const float scale_offset = scale_index - octave_offset * scale_length;
  return kNumSemitones * octave_offset +
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
