#ifndef BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
#define BARELYMUSICIAN_DSP_BIT_CRUSHER_H_

#include <algorithm>
#include <cassert>
#include <cmath>

#include "barelymusician.h"

namespace barely::internal {

/// Bit crusher effect with bit depth and sample rate reduction.
class BitCrusher {
 public:
  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @return Filtered output sample.
  [[nodiscard]] float Next(float input, float max_value, float step) noexcept {
    assert(max_value >= 0.0f);
    assert(step >= 0.0f);
    increment_ += step;
    if (increment_ >= 1.0f) {
      output_ = (max_value > 0.0f) ? std::round(input * max_value) / max_value : input;
      // output_ = 2.0f * std::round(0.5f * (input + 1.0f) * max_value) / max_value - 1.0f;
      increment_ -= 1.0f;
    }
    return output_;
  }

  /// Resets the filter output.
  void Reset() noexcept {
    increment_ = 0.0f;
    output_ = 0.0f;
  }

 private:
  float increment_ = 0.0f;
  float output_ = 0.0f;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_BIT_CRUSHER_H_
