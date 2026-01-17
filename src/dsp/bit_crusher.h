#ifndef BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
#define BARELYMUSICIAN_DSP_BIT_CRUSHER_H_

#include <barelymusician.h>

#include <cassert>
#include <cmath>

namespace barely {

// Bit crusher effect with bit depth and sample rate reduction.
class BitCrusher {
 public:
  [[nodiscard]] float Next(float input, float range, float increment) noexcept {
    assert(range >= 0.0f);
    assert(increment >= 0.0f);
    phase_ += increment;
    if (phase_ >= 1.0f) {
      output_ = (range > 0.0f) ? (std::round(input * range) / range) : input;
      phase_ -= 1.0f;
    }
    return output_;
  }

  void Reset() noexcept {
    output_ = 0.0f;
    phase_ = 0.0f;
  }

 private:
  // Last output sample.
  float output_ = 0.0f;

  // Sample step phase.
  float phase_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
