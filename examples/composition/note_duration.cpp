#include "examples/composition/note_duration.h"

#include <cassert>
#include <cmath>

namespace barely {

namespace {

// Linearly interpolates between given points.
double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

double GetPosition(int step, int num_steps) noexcept {
  assert(step >= 0);
  assert(num_steps > 0);
  const double num_beats = static_cast<double>(
      step / num_steps);  // NOLINT(bugprone-integer-division)
  return num_beats +
         static_cast<double>(step % num_steps) / static_cast<double>(num_steps);
}

double QuantizePosition(double position, double resolution,
                        double amount) noexcept {
  assert(resolution > 0.0);
  assert(amount >= 0.0 && amount <= 1.0);
  return Lerp(position, resolution * std::round(position / resolution), amount);
}

}  // namespace barely
