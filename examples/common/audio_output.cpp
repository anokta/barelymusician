
#include "examples/common/audio_output.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include "portaudio.h"

namespace barely::examples {

AudioOutput::AudioOutput() noexcept { Pa_Initialize(); }

AudioOutput::~AudioOutput() noexcept { Pa_Terminate(); }

void AudioOutput::Start(int sample_rate, int num_channels,
                        int num_frames) noexcept {
  assert(sample_rate >= 0);
  assert(num_channels >= 0);
  assert(num_frames >= 0);
  if (stream_) {
    // Stop the existing `stream_` first.
    Stop();
  }

  PaStreamParameters output_parameters;
  output_parameters.device = Pa_GetDefaultOutputDevice();
  assert(output_parameters.device != paNoDevice);
  output_parameters.channelCount = num_channels;
  output_parameters.sampleFormat = paFloat32;
  output_parameters.suggestedLatency =
      Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
  output_parameters.hostApiSpecificStreamInfo = nullptr;

  process_data_.buffer.resize(num_channels * num_frames);
  const auto callback =
      [](const void* /*input_buffer*/, void* output_buffer,
         unsigned long /*frames_per_buffer*/,  // NOLINT(google-runtime-int)
         const PaStreamCallbackTimeInfo* /*time_info*/,
         PaStreamCallbackFlags /*status_flags*/, void* user_data) noexcept {
        if (user_data) {
          // Access the audio process data via `user_data` to avoid capture.
          auto& process_data = *static_cast<ProcessData*>(user_data);
          if (process_data.callback) {
            float* output = static_cast<float*>(output_buffer);
            process_data.callback(process_data.buffer.data());
            for (const double sample : process_data.buffer) {
              *output++ = static_cast<float>(sample);
            }
          }
        }
        return static_cast<int>(paContinue);
      };
  Pa_OpenStream(&stream_, nullptr, &output_parameters, sample_rate, num_frames,
                paClipOff, callback, static_cast<void*>(&process_data_));
  Pa_StartStream(stream_);
}

void AudioOutput::Stop() noexcept {
  if (stream_) {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
  }
  stream_ = nullptr;
}

void AudioOutput::SetProcessCallback(
    ProcessCallback process_callback) noexcept {
  process_data_.callback = std::move(process_callback);
}

}  // namespace barely::examples
