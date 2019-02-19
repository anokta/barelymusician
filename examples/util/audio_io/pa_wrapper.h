#ifndef EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_
#define EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_

#include <functional>

#include "portaudio.h"

namespace barelyapi {
namespace examples {

// Simple portaudio wrapper to be used in the demo projects.
// @note Only a single instance allowed per application.
// TODO(#5): Revisit the single instance limitation (avoid static function).
class PaWrapper {
 public:
  // Alias for the audio process callback function.
  typedef std::function<void(float*)> AudioProcessCallback;

  PaWrapper();
  ~PaWrapper();

  // Initializes audio processing routine with the given configuration.
  //
  // @param sample_rate System sampling rate.
  // @param num_channels System number of output channels.
  // @param frames_per_buffer System number of frames per buffer.
  void Initialize(int sample_rate, int num_channels, int frames_per_buffer);

  // Shuts down the audio processing routine.
  void Shutdown();

  // Sets the audio process callback.
  //
  // @param audio_process Audio process callback function.
  void SetAudioProcessCallback(AudioProcessCallback&& audio_process);

 private:
  // Internal audio process method to trigger |callback_|.
  static int AudioProcess(const void* input, void* output,
                          unsigned long frames_per_buffer,
                          const PaStreamCallbackTimeInfo* time_info,
                          PaStreamCallbackFlags status, void* user_data);

  // Audio process callback function.
  static AudioProcessCallback audio_process_;

  // Stream for audio processing.
  PaStream* stream_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_