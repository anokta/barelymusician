#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

// Filter types.
enum class FilterType {
  // Low-pass filter.
  kLowPass = 0,
  // High-pass filter.
  kHighPass,
};

// One-pole filter that processes basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  template <FilterType kType>
  [[nodiscard]] float Next(float input, float coefficient) noexcept {
    assert(coefficient >= 0.0f);
    assert(coefficient <= 1.0f);
    output_ = coefficient * (output_ - input) + input;
    if constexpr (kType == FilterType::kHighPass) {
      return input - output_;
    } else {
      return output_;
    }
  }

  void Reset() noexcept { output_ = 0.0f; }

 private:
  float output_ = 0.0f;
};

inline float GetFilterCoeff(float sample_rate, float cutoff_freq) noexcept {
  assert(sample_rate > 0.0f);
  assert(cutoff_freq >= 0.0f);
  assert(cutoff_freq <= sample_rate);
  return std::exp(-2.0f * std::numbers::pi_v<float> * cutoff_freq / sample_rate);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
