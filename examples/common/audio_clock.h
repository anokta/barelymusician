#ifndef EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>
#include <cstdint>

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
  /// @return Timestamp in frames.
  std::int64_t GetTimestamp() const noexcept;

  /// Updates the clock.
  ///
  /// @param frame_count Number of frames to iterate.
  void Update(int frame_count) noexcept;

 private:
  // Frame rate in hertz.
  const int frame_rate_;

  // Monothonic timestamp in frames.
  std::atomic<std::int64_t> timestamp_ = 0;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_CLOCK_H_
