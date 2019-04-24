#ifndef EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_
#define EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_

#include <functional>

#include "portaudio.h"

namespace barelyapi {
namespace examples {

// Simple portaudio wrapper to be used in the demo projects.
class PaWrapper {
 public:
  // Audio process callback signature.
  using AudioProcessCallback = std::function<void(float*)>;

  PaWrapper();
  ~PaWrapper();

  // Initializes audio processing routine with the given configuration.
  //
  // @param sample_rate System sampling rate.
  // @param num_channels System number of output channels.
  // @param num_frames System number of frames per buffer.
  void Initialize(int sample_rate, int num_channels, int num_frames);

  // Shuts down the audio processing routine.
  void Shutdown();

  // Sets the audio process callback.
  //
  // @param audio_process Audio process callback.
  void SetAudioProcessCallback(AudioProcessCallback&& audio_process);

 private:
  // Audio process callback.
  AudioProcessCallback audio_process_;

  // Stream for audio processing.
  PaStream* stream_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_UTIL_AUDIO_IO_PA_WRAPPER_H_
