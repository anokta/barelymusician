#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/filter.h"

namespace barely {

/// One-pole filter type.
enum class FilterType {
  kLowPass,   // Low-pass filter.
  kHighPass,  // High-pass filter.
};

/// One-pole filter that features basic low-pass and high-pass filtering.
class OnePoleFilter : public Filter {
 public:
  /// Implements `Processor`.
  Real Next(Real input) noexcept override;
  void Reset() noexcept override;

  /// Sets the coefficent of the filter.
  ///
  /// @param coefficient Filter coefficient.
  void SetCoefficient(Real coefficient) noexcept;

  /// Sets the type of the filter.
  ///
  /// @param type Filter type.
  void SetType(FilterType type) noexcept;

 private:
  // Transfer function coefficient of the filter.
  Real coefficient_ = 1.0;

  // The last output sample.
  Real output_ = 0.0;

  // Filter type.
  FilterType type_ = FilterType::kLowPass;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
