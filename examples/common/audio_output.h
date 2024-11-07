#ifndef EXAMPLES_COMMON_AUDIO_OUTPUT_H_
#define EXAMPLES_COMMON_AUDIO_OUTPUT_H_

#include <functional>
#include <span>
#include <vector>

#include "miniaudio.h"

namespace barely::examples {

/// Simple wrapper for handling audio output.
class AudioOutput {
 public:
  /// Audio process callback signature.
  ///
  /// @param output_samples Span of mono output samples.
  using ProcessCallback = std::function<void(std::span<double> output_samples)>;

  /// Constructs new `AudioOutput`.
  ///
  /// @param sample_rate Sampling rate in hertz.
  /// @param sample_count Number of samples per buffer.
  AudioOutput(int sample_rate, int sample_count) noexcept;

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
  // Process data.
  struct ProcessData {
    // Buffer.
    std::vector<double> buffer;

    // Callback.
    ProcessCallback callback = nullptr;
  };
  ProcessData process_data_;

  // Audio device.
  ma_device device_;
};

}  // namespace barely::examples

#endif  // EXAMPLES_COMMON_AUDIO_OUTPUT_H_
