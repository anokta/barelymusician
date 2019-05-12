#ifndef EXAMPLES_AUDIO_OUTPUT_PA_AUDIO_OUTPUT_H_
#define EXAMPLES_AUDIO_OUTPUT_PA_AUDIO_OUTPUT_H_

#include "audio_output/audio_output.h"
#include "portaudio.h"

namespace barelyapi {
namespace examples {

// Simple portaudio wrapper to be used in the demo projects.
class PaAudioOutput : public AudioOutput {
 public:
  PaAudioOutput();
  ~PaAudioOutput();

  // Implements |AudioOutput|.
  void Start(int sample_rate, int num_channels, int num_frames) override;
  void Stop() override;
  void SetProcessCallback(ProcessCallback&& process_callback) override;

 private:
  // Process callback.
  ProcessCallback process_callback_;

  // Stream for audio processing.
  PaStream* stream_;
};

}  // namespace examples
}  // namespace barelyapi

#endif  // EXAMPLES_AUDIO_OUTPUT_PA_AUDIO_OUTPUT_H_
