#ifndef BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
#define BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_

namespace barelyapi {

// One-pole filter type.
enum class FilterType {
  kLowPass = 0,   // Low-pass filter.
  kHighPass = 1,  // High-pass filter.
};

// One-pole filter that features basic low-pass and high-pass filtering.
class OnePoleFilter {
 public:
  // Constructs new |OnePoleFilter|.
  explicit OnePoleFilter();

  // Processes the next input sample.
  //
  // @param input Input sample.
  // @return Processed output sample.
  float ProcessNext(float input);

  // Sets the coefficent of the filter.
  //
  // @param coefficient Filter coefficient.
  void SetCoefficient(float coefficient);

  // Sets the type of the filter.
  //
  // @param type Filter type.
  void SetType(FilterType type);

 private:
  // Transfer function coefficient of the filter.
  float coefficient_;

  // Filter type.
  FilterType type_;

  // The last output sample.
  float output_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_ONE_POLE_FILTER_H_
