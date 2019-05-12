#ifndef EXAMPLES_AUDIO_OUTPUT_AUDIO_OUTPUT_H_
#define EXAMPLES_AUDIO_OUTPUT_AUDIO_OUTPUT_H_

#include <functional>

namespace barelyapi {
namespace examples {

// Real-time audio output interface.
class AudioOutput {
 public:
  // Audio process callback signature.
  using ProcessCallback = std::function<void(float*)>;

  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~AudioOutput() = default;

  // Starts audio processing routine with the given configuration.
  //
  // @param sample_rate Sampling rate in Hz.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames per buffer.
  virtual void Start(int sample_rate, int num_channels, int num_frames) = 0;

  // Stops the audio processing routine.
  virtual void Stop() = 0;

  // Sets the audio process callback.
  //
  // @param process_callback Audio process callback.
  virtual void SetProcessCallback(ProcessCallback&& process_callback) = 0;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_AUDIO_OUTPUT_AUDIO_OUTPUT_H_
