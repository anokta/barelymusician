#include "examples/instruments/drumkit_instrument.h"

#include <algorithm>

namespace barelyapi::examples {

namespace {

// Default values.
const float kDefaultGain = 0.5f;
const float kDefaultRelease = 0.1f;

}  // namespace

DrumkitInstrument::DrumkitInstrument(int sample_rate)
    : sample_rate_(sample_rate), gain_(kDefaultGain) {}

void DrumkitInstrument::NoteOff(float pitch) {
  if (const auto it = voices_.find(pitch); it != voices_.cend()) {
    it->second.Stop();
  }
}

void DrumkitInstrument::NoteOn(float pitch, float intensity) {
  if (auto it = voices_.find(pitch); it != voices_.end()) {
    it->second.set_gain(intensity);
    it->second.Start();
  }
}

void DrumkitInstrument::Process(float* output, int num_channels,
                                int num_frames) {
  float mono_sample = 0.0f;
  for (int frame = 0; frame < num_frames; ++frame) {
    mono_sample = 0.0f;
    for (auto& voice : voices_) {
      mono_sample += voice.second.Next(0);
    }
    mono_sample *= gain_;
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void DrumkitInstrument::SetCustomData(void* data) {
  auto* drumkit_files =
      reinterpret_cast<std::unordered_map<float, WavFile>*>(data);
  for (const auto& [index, file] : *drumkit_files) {
    Add(index, file);
  }
}

InstrumentDefinition DrumkitInstrument::GetDefinition() {
  return GetInstrumentDefinition<DrumkitInstrument>(
      [](int sample_rate) { return DrumkitInstrument(sample_rate); });
}

void DrumkitInstrument::Add(float pitch, const WavFile& wav_file) {
  DrumkitVoice voice(sample_rate_);
  voice.envelope().SetRelease(kDefaultRelease);
  const auto& data = wav_file.GetData();
  const int data_size = static_cast<int>(data.size());
  voice.generator().SetData(data.data(), wav_file.GetSampleRate(), data_size);
  voices_.insert({pitch, voice});
}

}  // namespace barelyapi::examples
