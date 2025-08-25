
#include "common/audio_output.h"

#include <cassert>
#include <utility>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace barely::examples {

// NOLINTNEXTLINE(bugprone-exception-escape)
AudioOutput::AudioOutput(int sample_rate, int channel_count, int frame_count) noexcept
    : output_channels_(channel_count), output_samples_(channel_count * frame_count) {
  assert(sample_rate > 0);
  assert(channel_count > 0);
  assert(frame_count > 0);
  // Initialize output channels.
  for (int i = 0; i < channel_count; ++i) {
    output_channels_[i] = &output_samples_[i * frame_count];
  }
  // Configure the playback device.
  ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
  device_config.playback.format = ma_format_f32;
  device_config.playback.channels = static_cast<ma_uint32>(channel_count);
  device_config.periodSizeInFrames = static_cast<ma_uint32>(frame_count);
  device_config.sampleRate = static_cast<ma_uint32>(sample_rate);
  device_config.pUserData = static_cast<void*>(this);
  device_config.dataCallback = [](ma_device* device, void* output, const void* /*input*/,
                                  ma_uint32 frame_count) noexcept {
    assert(device != nullptr);
    assert(device->pUserData != nullptr);
    if (auto& audio_output = *static_cast<AudioOutput*>(device->pUserData);
        audio_output.process_callback_) {
      assert(device->playback.channels <= audio_output.output_channels_.size());
      assert(device->playback.channels * frame_count <= audio_output.output_samples_.size());
      audio_output.process_callback_(audio_output.output_channels_, static_cast<int>(frame_count));
      for (int frame = 0; frame < static_cast<int>(frame_count); ++frame) {
        for (int i = 0; i < static_cast<int>(device->playback.channels); ++i) {
          static_cast<float*>(output)[frame * device->playback.channels + i] =
              audio_output.output_channels_[i][frame];
        }
      }
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
