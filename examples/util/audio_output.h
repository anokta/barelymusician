#ifndef EXAMPLES_UTIL_AUDIO_OUTPUT_H_
#define EXAMPLES_UTIL_AUDIO_OUTPUT_H_

#include <functional>

#include "portaudio.h"

namespace barelyapi {
namespace examples {

// Simple portaudio wrapper for handling audio output.
class AudioOutput {
 public:
  // Audio process callback signature.
  using ProcessCallback = std::function<void(float* output)>;

  // Constructs new |AudioOutput|.
  AudioOutput();

  // Destructs |AudioOutput|.
  ~AudioOutput();

  // Starts audio processing routine with the given configuration.
  //
  // @param sample_rate Sampling rate in Hz.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames per buffer.
  void Start(int sample_rate, int num_channels, int num_frames);

  // Stops the audio processing routine.
  void Stop();

  // Sets the audio process callback.
  //
  // @param process_callback Audio process callback.
  void SetProcessCallback(ProcessCallback&& process_callback);

 private:
  // Process callback.
  ProcessCallback process_callback_;

  // Stream for audio processing.
  PaStream* stream_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_AUDIO_OUTPUT_H_