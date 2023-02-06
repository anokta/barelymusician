#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>

#include "barelymusician/barelymusician.h"

namespace barely::examples {

/// Thread-safe audio dsp clock.
class AudioClock {
 public:
  /// Constructs new `AudioClock`.
  explicit AudioClock(Integer frame_rate) noexcept;

  /// Returns the timestamp.
  ///
  /// @return Timestamp in seconds.
  Integer GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param frame_count Number of frames to iterate.
  void Update(Integer frame_count) noexcept;

 private:
  // Frame rate in hertz.
  const Integer frame_rate_;

  // Monothonic timestamp in seconds.
  std::atomic<Integer> timestamp_ = 0;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
