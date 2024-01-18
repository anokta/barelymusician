#ifndef BARELYMUSICIAN_DSP_GENERATOR_H_
#define BARELYMUSICIAN_DSP_GENERATOR_H_

namespace barely {

/// Unit generator interface that produces per sample PCM output.
class Generator {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Generator() = default;

  /// Generates the next output sample.
  ///
  /// @return Next output sample.
  virtual float Next() noexcept = 0;

  /// Resets module state.
  virtual void Reset() noexcept = 0;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_DSP_GENERATOR_H_
