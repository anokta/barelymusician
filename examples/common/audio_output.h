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
  /// @param channel_count Number of channels.
  /// @param frame_count Number of  frames.
  using ProcessCallback = std::function<void(float* samples, int channel_count, int frame_count)>;

  /// Constructs new `AudioOutput`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param channel_count Number of channels.
  /// @param frame_count Number of frames.
  AudioOutput(int frame_rate, int channel_count, int frame_count) noexcept;

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

  // Number of channels.
  int channel_count_ = 0;

  // Process callback.
  ProcessCallback process_callback_ = nullptr;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMMON_AUDIO_OUTPUT_H_
