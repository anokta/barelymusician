#ifndef BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_
#define BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_

#include <functional>

#include "miniaudio.h"

namespace barely::examples {

/// Simple wrapper for handling audio output.
class AudioOutput {
 public:
  /// Audio process callback signature.
  ///
  /// @param samples Array of interleaved samples.
  /// @param frame_count Number of  frames.
  using ProcessCallback = std::function<void(float* samples, int frame_count)>;

  /// Constructs new `AudioOutput`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param frame_count Number of frames.
  AudioOutput(int sample_rate, int frame_count) noexcept;

  /// Destructs `AudioOutput`.
  ~AudioOutput() noexcept;

  /// Non-copyable and non-movable.
  AudioOutput(const AudioOutput& other) noexcept = delete;
  AudioOutput& operator=(const AudioOutput& other) noexcept = delete;
  AudioOutput(AudioOutput&& other) noexcept = delete;
  AudioOutput& operator=(AudioOutput&& other) noexcept = delete;

  /// Starts audio processing routine.
  void Start();

  /// Stops the audio processing routine.
  void Stop() noexcept;

  /// Sets the audio process callback.
  ///
  /// @param process_callback Audio process callback.
  void SetProcessCallback(ProcessCallback process_callback) noexcept;

 private:
  // Audio device.
  ma_device device_;

  // Process callback.
  ProcessCallback process_callback_ = nullptr;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_
