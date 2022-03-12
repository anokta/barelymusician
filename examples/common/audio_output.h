#ifndef EXAMPLES_COMMON_AUDIO_OUTPUT_H_
#define EXAMPLES_COMMON_AUDIO_OUTPUT_H_

#include <functional>

#include "portaudio.h"

namespace barely::examples {

/// Simple portaudio wrapper for handling audio output.
class AudioOutput {
 public:
  /// Audio process callback signature.
  ///
  /// @param output Output buffer.
  using ProcessCallback = std::function<void(double* output)>;

  /// Constructs new `AudioOutput`.
  AudioOutput() noexcept;

  /// Destructs `AudioOutput`.
  ~AudioOutput() noexcept;

  /// Non-copyable and non-movable.
  AudioOutput(const AudioOutput& other) = delete;
  AudioOutput& operator=(const AudioOutput& other) = delete;
  AudioOutput(AudioOutput&& other) noexcept = delete;
  AudioOutput& operator=(AudioOutput&& other) noexcept = delete;

  /// Starts audio processing routine with the given configuration.
  ///
  /// @param sample_rate Sampling rate in hz.
  /// @param num_channels Number of output channels.
  /// @param num_frames Number of output frames per buffer.
  void Start(int sample_rate, int num_channels, int num_frames) noexcept;

  /// Stops the audio processing routine.
  void Stop() noexcept;

  /// Sets the audio process callback.
  ///
  /// @param process_callback Audio process callback.
  void SetProcessCallback(ProcessCallback process_callback) noexcept;

 private:
  // Process callback.
  ProcessCallback process_callback_ = nullptr;

  // Stream for audio processing.
  PaStream* stream_ = nullptr;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_OUTPUT_H_
