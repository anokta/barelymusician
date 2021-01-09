#ifndef BARELYMUSICIAN_DSP_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_PROCESSOR_H_

namespace barelyapi {

/// Unit filter interface that processes per sample PCM input.
class Filter {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Filter() = default;

  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @return Filtered output sample.
  virtual float Next(float input) = 0;

  /// Resets module state.
  virtual void Reset() = 0;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_DSP_PROCESSOR_H_
