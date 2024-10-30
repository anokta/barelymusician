#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

namespace barely::internal {

/// One-pole filter type.
enum class FilterType {
  kLowPass,   // Low-pass filter.
  kHighPass,  // High-pass filter.
};

/// One-pole filter that features basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @return Filtered output sample.
  double Next(double input) noexcept;

  /// Resets module state.
  void Reset() noexcept;

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

  // The last output sample.
  double output_ = 0.0;

  // Filter type.
  FilterType type_ = FilterType::kLowPass;
};

/// Returns one-pole filter coefficient for a given cutoff frequency.
///
/// @param frame_rate Frame rate in hertz.
/// @param cuttoff_frequency Cutoff frequency in hertz.
/// @return Filter coefficient.
double GetFilterCoefficient(int frame_rate, double cuttoff_frequency) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
