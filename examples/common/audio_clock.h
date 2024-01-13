#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>
#include <cstdint>

#include "barelymusician/barelymusician.h"

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
  Rational GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param frame_count Number of frames to iterate.
  void Update(int frame_count) noexcept;

 private:
  // Frame rate in hertz.
  const int frame_rate_;

  // Monothonic tick in frames.
  std::atomic<std::int64_t> tick_ = 0;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
