#include "barelymusician/composition/duration.h"

#include <cassert>
#include <cmath>

namespace barely {

namespace {

// Linearly interpolates between given points.
double Lerp(double a, double b, double t) { return a + t * (b - a); }

}  // namespace

double GetPosition(int step, int step_count) noexcept {
  assert(step >= 0);
  assert(step_count > 0);
  // NOLINTNEXTLINE(bugprone-integer-division)
  const double beat_count = static_cast<double>(step / step_count);
  return beat_count + static_cast<double>(step % step_count) /
                          static_cast<double>(step_count);
}

double QuantizePosition(double position, double resolution,
                        double amount) noexcept {
  assert(resolution > 0.0);
  assert(amount >= 0.0 && amount <= 1.0);
  return Lerp(position, resolution * std::round(position / resolution), amount);
}

}  // namespace barely