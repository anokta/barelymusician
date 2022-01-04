#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>

namespace barely::examples {

/// Thread-safe audio dsp clock.
class AudioClock {
 public:
  /// Constructs new |AudioClock|.
  explicit AudioClock(int sample_rate) noexcept;

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param num_frames Number of frames to iterate.
  void Update(int num_frames) noexcept;

 private:
  // Sampling rate in hz.
  const int sample_rate_;

  // Monothonic timestamp in seconds.
  std::atomic<double> timestamp_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
