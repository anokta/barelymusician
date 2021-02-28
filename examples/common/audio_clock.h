#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>

namespace barelyapi::examples {

/// Thread-safe audio dsp clock.
class AudioClock {
 public:
  /// Constructs new |AudioClock|.
  explicit AudioClock(int sample_rate);

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  double GetTimestamp() const;

  /// Updates the clock.
  ///
  /// @param num_frames Number of frames to iterate.
  void Update(int num_frames);

 private:
  // Sampling rate in Hz.
  const int sample_rate_;

  // Monothonic timestamp in seconds.
  std::atomic<double> timestamp_;
};

}  // namespace barelyapi::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
