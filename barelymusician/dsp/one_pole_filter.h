#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

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
  /// Constructs new |OnePoleFilter|.
  OnePoleFilter();

  /// Implements |Processor|.
  float Next(float input) override;
  void Reset() override;

  /// Sets the coefficent of the filter.
  ///
  /// @param coefficient Filter coefficient.
  void SetCoefficient(float coefficient);

  /// Sets the type of the filter.
  ///
  /// @param type Filter type.
  void SetType(FilterType type);

 private:
  // Transfer function coefficient of the filter.
  float coefficient_;

  // Filter type.
  FilterType type_;

  // The last output sample.
  float output_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
