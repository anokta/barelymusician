#ifndef BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
#define BARELYMUSICIAN_DSP_BIT_CRUSHER_H_

#include <cassert>
#include <cmath>

#include "barelymusician.h"

namespace barely::internal {

/// Bit crusher effect with bit depth and sample rate reduction.
class BitCrusher {
 public:
  /// Applies the bit crusher effect to the next input sample.
  ///
  /// @param input Input sample.
  /// @param range Sample value range (for bit depth reduction).
  /// @param increment Sample step increment (for sample rate reduction).
  /// @return Filtered output sample.
  [[nodiscard]] float Next(float input, float range, float increment) noexcept {
    assert(range >= 0.0f);
    assert(increment >= 0.0f);
    phase_ += increment;
    if (phase_ >= 1.0f) {
      // Zero `range` is passed to disable the bit depth reduction.
      output_ = (range > 0.0f) ? std::round(input * range) / range : input;
      phase_ -= 1.0f;
    }
    return output_;
  }

  /// Resets the effect state.
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

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
