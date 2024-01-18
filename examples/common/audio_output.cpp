
#include "examples/common/audio_output.h"

#include <cassert>
#include <utility>

#include "portaudio.h"

namespace barely::examples {

AudioOutput::AudioOutput() noexcept { Pa_Initialize(); }

AudioOutput::~AudioOutput() noexcept { Pa_Terminate(); }

void AudioOutput::Start(int frame_rate, int channel_count, int frame_count) {
  assert(frame_rate >= 0);
  assert(channel_count >= 0);
  assert(frame_count >= 0);
  if (stream_) {
    // Stop the existing `stream_` first.
    Stop();
  }

  PaStreamParameters output_parameters;
  output_parameters.device = Pa_GetDefaultOutputDevice();
  assert(output_parameters.device != paNoDevice);
  output_parameters.channelCount = channel_count;
  output_parameters.sampleFormat = paFloat32;
  output_parameters.suggestedLatency =
      Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
  output_parameters.hostApiSpecificStreamInfo = nullptr;

  const auto callback = [](const void* /*input_buffer*/, void* output_buffer,
                           unsigned long /*frames_per_buffer*/,  // NOLINT(google-runtime-int)
                           const PaStreamCallbackTimeInfo* /*time_info*/,
                           PaStreamCallbackFlags /*status_flags*/, void* user_data) noexcept {
    if (user_data) {
      // Access the audio process data via `user_data` to avoid capture.
      if (auto& process_callback = *static_cast<ProcessCallback*>(user_data); process_callback) {
        process_callback(static_cast<float*>(output_buffer));
      }
    }
    return static_cast<int>(paContinue);
  };
  Pa_OpenStream(&stream_, nullptr, &output_parameters, frame_rate, frame_count, paClipOff, callback,
                static_cast<void*>(&process_callback_));
  Pa_StartStream(stream_);
}

void AudioOutput::Stop() noexcept {
  if (stream_) {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
  }
  stream_ = nullptr;
}

void AudioOutput::SetProcessCallback(ProcessCallback process_callback) noexcept {
  process_callback_ = std::move(process_callback);
}

}  // namespace barely::examples
