#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include <barelymusician.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

namespace barely {

// One-pole filter that processes basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
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

  void Reset() noexcept { output_ = 0.0f; }

 private:
  // The last output sample.
  float output_ = 0.0f;
};

inline float GetFilterCoefficient(float sample_rate, float cuttoff_frequency) noexcept {
  assert(sample_rate > 0.0f);
  assert(cuttoff_frequency >= 0.0f);
  // c = exp(-2 * pi * fc / fs).
  return std::clamp(std::exp(-2.0f * std::numbers::pi_v<float> * cuttoff_frequency / sample_rate),
                    0.0f, 1.0f);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
