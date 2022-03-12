#ifndef BARELYMUSICIAN_PRESETS_DSP_VOICE_H_
#define BARELYMUSICIAN_PRESETS_DSP_VOICE_H_

namespace barelyapi {

/// Instrument voice interface.
class Voice {
 public:
  /// Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Voice() = default;

  /// Returns whether the voice is currently active (i.e., playing).
  ///
  /// @return True if active.
  [[nodiscard]] virtual bool IsActive() const noexcept = 0;

  /// Returns the next output sample for the given output channel.
  ///
  /// @param channel Output channel.
  /// @return Output sample.
  virtual double Next(int channel) noexcept = 0;

  /// Starts the voice.
  virtual void Start() noexcept = 0;

  /// Stops the voice.
  virtual void Stop() noexcept = 0;
};

}  // namespace barelyapi

#endif  // EXAMPLES_VOICE_VOICE_H_
