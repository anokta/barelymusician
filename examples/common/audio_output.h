#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_

#include <functional>
#include <utility>

#include "miniaudio.h"

namespace barely::examples {

// Simple wrapper for handling audio output.
class AudioOutput {
 public:
  using ProcessCallback =
      std::function<void(float* output_samples, int output_channel_count, int output_frame_count)>;

  AudioOutput(int sample_rate, int channel_count, int frame_count) noexcept;
  ~AudioOutput() noexcept;

  /// Non-copyable and non-movable.
  AudioOutput(const AudioOutput& other) noexcept = delete;
  AudioOutput& operator=(const AudioOutput& other) noexcept = delete;
  AudioOutput(AudioOutput&& other) noexcept = delete;
  AudioOutput& operator=(AudioOutput&& other) noexcept = delete;

  void Start();
  void Stop() noexcept;

  void SetProcessCallback(ProcessCallback process_callback) noexcept {
    process_callback_ = std::move(process_callback);
  }

 private:
  ma_device device_;
  ProcessCallback process_callback_ = nullptr;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_
