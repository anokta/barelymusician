#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>

namespace barely::examples {

/// Thread-safe audio dsp clock.
class AudioClock {
 public:
  /// Constructs new `AudioClock`.
  ///
  /// @param frame_rate Frame rate in hertz.
  explicit AudioClock(int frame_rate) noexcept;

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param frame_count Number of frames to iterate.
  void Update(int frame_count) noexcept;

 private:
  // Frame rate in hertz.
  const int frame_rate_;

  // Monothonic timestamp in seconds.
  std::atomic<double> timestamp_;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_
