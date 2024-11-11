#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

#include "barelymusician.h"

namespace barely::internal {

/// Filter callback signature alias.
///
/// @param input Input sample.
/// @param coefficient Filter coefficient.
/// @param state Mutable filter state.
/// @return Filtered output sample.
using FilterCallback = double (*)(double input, double coefficient, double& state);

/// Filters the next input sample.
///
/// @tparam kType Filter type.
/// @param input Input sample.
/// @param coefficient Filter coefficient.
/// @param state Mutable filter state.
/// @return Filtered output sample.
template <FilterType kType>
double Filter(double input, [[maybe_unused]] double coefficient, double& state) noexcept {
  if constexpr (kType == FilterType::kNone) {
    state = input;
  } else {
    assert(coefficient >= 0.0);
    assert(coefficient <= 1.0);
    state = coefficient * (state - input) + input;
  }
  if constexpr (kType == FilterType::kHighPass) {
    return input - state;
  } else {
    return state;
  }
}

/// Array of filter callbacks for each shape.
inline constexpr std::array<FilterCallback, static_cast<int>(BarelyFilterType_kCount)>
    kFilterCallbacks = {
        &Filter<FilterType::kNone>,
        &Filter<FilterType::kLowPass>,
        &Filter<FilterType::kHighPass>,
};

/// Returns the corresponding one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
inline double GetFilterCoefficient(int sample_rate, double cuttoff_frequency) noexcept {
  assert(sample_rate > 0);
  assert(cuttoff_frequency >= 0.0);
  // c = exp(-2 * pi * fc / fs).
  // TODO(#8): Verify if this *a proper way* to calculate the coefficient?
  return std::clamp(std::exp(-2.0 * std::numbers::pi_v<double> * cuttoff_frequency /
                             static_cast<double>(sample_rate)),
                    0.0, 1.0);
}

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
