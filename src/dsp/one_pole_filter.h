#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

/// One-pole filter that processes basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  /// Filters the next input sample.
  ///
  /// @tparam kType Filter type.
  /// @param input Input sample.
  /// @param coefficient Filter coefficient.
  /// @return Filtered output sample.
  template <FilterType kType>
  [[nodiscard]] float Next(float input, float coefficient) noexcept {
    static_assert(kType != FilterType::kNone, "Invalid filter type");
    assert(coefficient >= 0.0f);
    assert(coefficient <= 1.0f);
    output_ = coefficient * (output_ - input) + input;
    if constexpr (kType == FilterType::kHighPass) {
      return input - output_;
    } else {
      return output_;
    }
  }

  /// Resets the filter output.
  void Reset() noexcept { output_ = 0.0f; }

 private:
  // The last output sample.
  float output_ = 0.0f;
};

/// Returns the corresponding one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
inline float GetFilterCoefficient(int sample_rate, float cuttoff_frequency) noexcept {
  assert(sample_rate > 0);
  assert(cuttoff_frequency >= 0.0f);
  // c = exp(-2 * pi * fc / fs).
  return std::clamp(std::exp(-2.0f * std::numbers::pi_v<float> * cuttoff_frequency /
                             static_cast<float>(sample_rate)),
                    0.0f, 1.0f);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
