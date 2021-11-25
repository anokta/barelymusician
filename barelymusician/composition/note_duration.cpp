#include "barelymusician/composition/note_duration.h"

#include <cmath>

#include "barelymusician/common/logging.h"

namespace barely {

namespace {

// Linearly interpolates between given points.
double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

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

}  // namespace barely
