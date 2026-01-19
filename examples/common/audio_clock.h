#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_

#include <atomic>
#include <cassert>

namespace barely::examples {

// Thread-safe audio dsp clock.
class AudioClock {
 public:
  explicit AudioClock(int sample_rate) noexcept
      : sample_rate_(static_cast<double>(sample_rate)), timestamp_(0.0) {
    assert(sample_rate > 0);
  }

  void Update(int frame_count) noexcept {
    assert(frame_count >= 0);
    timestamp_ = timestamp_ + static_cast<double>(frame_count) / sample_rate_;
  }

  double GetTimestamp() const noexcept { return timestamp_; }

 private:
  double sample_rate_;
  std::atomic<double> timestamp_;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_CLOCK_H_
