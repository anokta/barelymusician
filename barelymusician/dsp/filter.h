#ifndef BARELYMUSICIAN_DSP_PROCESSOR_H_
#define BARELYMUSICIAN_DSP_PROCESSOR_H_

#include "barelymusician/barelymusician.h"

namespace barely {

/// Unit filter interface that processes per sample PCM input.
class Filter {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Filter() = default;

  /// Filters the next input sample.
  ///
  /// @param input Input sample.
  /// @return Filtered output sample.
  virtual Real Next(Real input) noexcept = 0;

  /// Resets module state.
  virtual void Reset() noexcept = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_PROCESSOR_H_
