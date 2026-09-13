#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace barely {

// Filter types.
enum class FilterType : uint8_t {
  // Low-pass filter.
  kLowPass = 0,
  // High-pass filter.
  kHighPass,
};

// One-pole filter that processes basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  template <FilterType kType>
  [[nodiscard]] double Next(double input, double coefficient) noexcept {
    assert(coefficient >= 0.0);
    assert(coefficient <= 1.0);
    output_ = coefficient * (output_ - input) + input;
    if constexpr (kType == FilterType::kHighPass) {
      return input - output_;
    } else {
      return output_;
    }
  }

  void Reset() noexcept { output_ = 0.0; }

 private:
  double output_ = 0.0;
};

inline double GetFilterCoeff(double sample_rate, double cutoff_freq) noexcept {
  assert(sample_rate > 0.0);
  assert(cutoff_freq >= 0.0);
  assert(cutoff_freq <= sample_rate);
  return std::exp(-2.0 * std::numbers::pi_v<double> * cutoff_freq / sample_rate);
}

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
