#ifndef BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
#define BARELYMUSICIAN_DSP_BIT_CRUSHER_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Bit crusher effect with bit depth and sample rate reduction.
class BitCrusher {
 public:
  [[nodiscard]] double Next(double input, double range, double increment) noexcept {
    assert(range >= 0.0);
    assert(increment >= 0.0);
    phase_ += increment;
    if (phase_ >= 1.0) {
      output_ = (range > 0.0) ? (std::round(input * range) / range) : input;
      phase_ -= 1.0;
    }
    return output_;
  }

  void Reset() noexcept {
    output_ = 0.0;
    phase_ = 0.0;
  }

 private:
  double output_ = 0.0;
  double phase_ = 0.0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
