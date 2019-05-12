#include "audio_output/pa_audio_output.h"

#include <utility>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace examples {

PaAudioOutput::PaAudioOutput() : process_callback_(nullptr), stream_(nullptr) {
  Pa_Initialize();
}

PaAudioOutput::~PaAudioOutput() { Pa_Terminate(); }

void PaAudioOutput::Start(int sample_rate, int num_channels, int num_frames) {
  DCHECK_GE(sample_rate, 0);
  DCHECK_GE(num_channels, 0);
  DCHECK_GE(num_frames, 0);

  if (stream_ != nullptr) {
    // Stop the existing |stream_| first.
    Stop();
  }

  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = num_channels;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = nullptr;

  const auto callback = [](const void* input_buffer, void* output_buffer,
                           unsigned long frames_per_buffer,
                           const PaStreamCallbackTimeInfo* time_info,
                           PaStreamCallbackFlags status_flags,
                           void* user_data) {
    if (user_data != nullptr) {
      // Access the audio process callback via |user_data| (to avoid capturing
      // |process_callback_|).
      const auto& process_callback =
          *reinterpret_cast<ProcessCallback*>(user_data);
      process_callback(reinterpret_cast<float*>(output_buffer));
    }
    return static_cast<int>(paContinue);
  };
  Pa_OpenStream(&stream_, nullptr, &outputParameters, sample_rate, num_frames,
                paClipOff, callback,
                reinterpret_cast<void*>(&process_callback_));
  Pa_StartStream(stream_);
}

void PaAudioOutput::Stop() {
  if (stream_ != nullptr) {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
  }
  stream_ = nullptr;
}

void PaAudioOutput::SetProcessCallback(ProcessCallback&& process_callback) {
  process_callback_ = std::move(process_callback);
}

}  // namespace examples
}  // namespace barelyapi
