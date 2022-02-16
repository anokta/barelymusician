#ifndef EXAMPLES_DSP_PROCESSOR_H_
#define EXAMPLES_DSP_PROCESSOR_H_

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
  virtual float Next(float input) noexcept = 0;

  /// Resets module state.
  virtual void Reset() noexcept = 0;
};

}  // namespace barelyapi

#endif  // EXAMPLES_DSP_PROCESSOR_H_
