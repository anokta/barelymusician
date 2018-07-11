#include "util/audio_io/pa_wrapper.h"

namespace barelyapi {
namespace examples {

PaWrapper::AudioProcessCallback PaWrapper::audio_process_ =
    PaWrapper::AudioProcessCallback();

PaWrapper::PaWrapper() : stream_(nullptr) { Pa_Initialize(); }

PaWrapper::~PaWrapper() { Pa_Terminate(); }

void PaWrapper::Initialize(int sample_rate, int num_channels,
                           int frames_per_buffer) {
  if (stream_ != nullptr) {
    return;
  }
  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = num_channels;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = nullptr;

  Pa_OpenStream(&stream_, nullptr, &outputParameters, sample_rate,
                frames_per_buffer, paClipOff, PaWrapper::AudioProcess, nullptr);
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

int PaWrapper::AudioProcess(const void* input, void* output,
                            unsigned long frames_per_buffer,
                            const PaStreamCallbackTimeInfo* time_info,
                            PaStreamCallbackFlags status, void* user_data) {
  if (audio_process_ != nullptr) {
    audio_process_(reinterpret_cast<float*>(output));
  }
  return static_cast<int>(paContinue);
}

}  // namespace examples
}  // namespace barelyapi
