#ifndef BARELYMUSICIAN_DSP_VOICE_H_
#define BARELYMUSICIAN_DSP_VOICE_H_

namespace barely {

/// Instrument voice interface.
class Voice {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Voice() = default;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  virtual bool IsActive() const = 0;

  /// Returns the next output sample for the given output channel.
  ///
  /// @param channel Output channel.
  /// @return Output sample.
  virtual float Next(int channel) = 0;

  /// Starts the voice.
  virtual void Start() = 0;

  /// Stops the voice.
  virtual void Stop() = 0;
};

}  // namespace barely

#endif  // EXAMPLES_VOICE_VOICE_H_
