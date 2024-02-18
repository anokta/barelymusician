#include "barelymusician/composition/duration.h"

#include <cassert>
#include <cmath>

namespace barely {

double QuantizePosition(double position, double resolution, double amount) noexcept {
  assert(resolution > 0.0);
  assert(amount >= 0.0 && amount <= 1.0);
  return std::lerp(position, resolution * std::round(position / resolution), amount);
}

}  // namespace barely
