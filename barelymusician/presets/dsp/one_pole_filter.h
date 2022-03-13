#ifndef BARELYMUSICIAN_PRESETS_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_PRESETS_DSP_ONE_POLE_FILTER_H_

#include "barelymusician/presets/dsp/filter.h"

namespace barelyapi {

/// One-pole filter type.
enum class FilterType {
  kLowPass,   // Low-pass filter.
  kHighPass,  // High-pass filter.
};

/// One-pole filter that features basic low-pass and high-pass filtering.
class OnePoleFilter : public Filter {
 public:
  /// Implements `Processor`.
  double Next(double input) noexcept override;
  void Reset() noexcept override;

  /// Sets the coefficent of the filter.
  ///
  /// @param coefficient Filter coefficient.
  void SetCoefficient(double coefficient) noexcept;

  /// Sets the type of the filter.
  ///
  /// @param type Filter type.
  void SetType(FilterType type) noexcept;

 private:
  // Transfer function coefficient of the filter.
  double coefficient_ = 1.0;

  // Filter type.
  FilterType type_ = FilterType::kLowPass;

  // The last output sample.
  double output_ = 0.0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_DSP_ONE_POLE_FILTER_H_