
#include "common/audio_output.h"

#include <cassert>
#include <utility>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
AudioOutput::AudioOutput(int sample_rate, int sample_count) noexcept {
  assert(sample_rate > 0);
  assert(sample_count > 0);
  // Configure the playback device.
  ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = ma_format_f32;
  device_config.playback.channels = 1;
  device_config.periodSizeInFrames = sample_count;
  device_config.sampleRate = sample_rate;
  device_config.pUserData = static_cast<void*>(&process_callback_);
  device_config.dataCallback = [](ma_device* device, void* output, const void* /*input*/,
                                  ma_uint32 frame_count) noexcept {
    if (auto& callback = *static_cast<ProcessCallback*>(device->pUserData); callback) {
      float* output_buffer = static_cast<float*>(output);
      callback({output_buffer, output_buffer + frame_count});
    }
  };
  // Initialize the device.
  [[maybe_unused]] const auto result = ma_device_init(nullptr, &device_config, &device_);
  assert(result == MA_SUCCESS);
}

AudioOutput::~AudioOutput() noexcept { ma_device_uninit(&device_); }

void AudioOutput::Start() {
  if (ma_device_start(&device_) != MA_SUCCESS) {
    Stop();
  }
}

void AudioOutput::Stop() noexcept { ma_device_stop(&device_); }

void AudioOutput::SetProcessCallback(ProcessCallback process_callback) noexcept {
  process_callback_ = std::move(process_callback);
}

}  // namespace barely::examples
