#ifndef EXAMPLES_COMMON_AUDIO_OUTPUT_H_
#define EXAMPLES_COMMON_AUDIO_OUTPUT_H_

#include <functional>
#include <vector>

#include "miniaudio.h"

namespace barely::examples {

/// Simple wrapper for handling audio output.
class AudioOutput {
 public:
  /// Audio process callback signature.
  ///
  /// @param output Output buffer.
  using ProcessCallback = std::function<void(double* output)>;

  /// Constructs new `AudioOutput`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param channel_count Number of output channels.
  /// @param frame_count Number of output frames per buffer.
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
