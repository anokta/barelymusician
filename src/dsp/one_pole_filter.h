#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include "barelymusician.h"

namespace barely::internal {

/// One-pole filter that features basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @param coefficient Filter coefficient.
  /// @param type Filter type.
  /// @return Filtered output sample.
  double Next(double input, double coefficient, FilterType type) noexcept;

  /// Resets module state.
  void Reset() noexcept;

 private:
  // The last output sample.
  double output_ = 0.0;
};

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param sample_rate Sampling rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
double GetFilterCoefficient(int sample_rate, double cuttoff_frequency) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
