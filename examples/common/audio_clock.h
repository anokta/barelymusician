#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>

namespace barely::examples {

/// Thread-safe audio dsp clock.
class AudioClock {
 public:
  /// Constructs new `AudioClock`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  explicit AudioClock(int sample_rate) noexcept;

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param sample_count Number of samples to iterate.
  void Update(int sample_count) noexcept;

 private:
  // Sampling rate in hertz.
  const int sample_rate_;

  // Monothonic timestamp in seconds.
  std::atomic<double> timestamp_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
