
#include "common/audio_output.h"

#include <cassert>
#include <utility>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
AudioOutput::AudioOutput(int frame_rate, int channel_count, int frame_count) noexcept
    : channel_count_(channel_count) {
  assert(frame_rate > 0);
  assert(channel_count > 0);
  assert(frame_count > 0);
  // Configure the playback device.
  ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = ma_format_f32;
  device_config.playback.channels = channel_count;
  device_config.periodSizeInFrames = frame_count;
  device_config.sampleRate = frame_rate;
  device_config.pUserData = static_cast<void*>(this);
  device_config.dataCallback = [](ma_device* device, void* output, const void* /*input*/,
                                  ma_uint32 frame_count) noexcept {
    assert(device->pUserData != nullptr);
    if (auto& audio_output = *static_cast<AudioOutput*>(device->pUserData);
        audio_output.process_callback_) {
      float* output_samples = static_cast<float*>(output);
      audio_output.process_callback_(output_samples, audio_output.channel_count_,
                                     static_cast<int>(frame_count));
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
