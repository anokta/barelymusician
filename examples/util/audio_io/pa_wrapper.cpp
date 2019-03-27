#include "util/audio_io/pa_wrapper.h"

#include <utility>

namespace barelyapi {
namespace examples {

PaWrapper::PaWrapper() : audio_process_(nullptr), stream_(nullptr) {
  Pa_Initialize();
}

PaWrapper::~PaWrapper() { Pa_Terminate(); }

void PaWrapper::Initialize(int sample_rate, int num_channels, int num_frames) {
  if (stream_ != nullptr) {
    // Shut down the existing |stream_| first.
    Shutdown();
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
      // Access the audio process callback via |user_data| (to avoid
      // capturing |audio_process_|).
      const auto& audio_process =
          *reinterpret_cast<AudioProcessCallback*>(user_data);
      audio_process(reinterpret_cast<float*>(output_buffer));
    }
    return static_cast<int>(paContinue);
  };
  Pa_OpenStream(&stream_, nullptr, &outputParameters, sample_rate, num_frames,
                paClipOff, callback, reinterpret_cast<void*>(&audio_process_));
  Pa_StartStream(stream_);
}

void PaWrapper::Shutdown() {
  if (stream_ != nullptr) {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
  }
  stream_ = nullptr;
}

void PaWrapper::SetAudioProcessCallback(AudioProcessCallback&& audio_process) {
  audio_process_ = std::move(audio_process);
}

}  // namespace examples
}  // namespace barelyapi
