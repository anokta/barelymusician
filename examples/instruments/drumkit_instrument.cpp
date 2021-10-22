#include "examples/instruments/drumkit_instrument.h"

#include <any>
#include <unordered_map>

#include "barelymusician/common/find_or_null.h"
#include "examples/common/wav_file.h"

namespace barelyapi::examples {

namespace {

// Default values.
const float kDefaultGain = 0.5f;
const float kDefaultRelease = 0.1f;

}  // namespace

DrumkitInstrument::DrumkitInstrument(int sample_rate)
    : sample_rate_(sample_rate), gain_(kDefaultGain) {}

void DrumkitInstrument::NoteOff(float pitch) {
  if (auto* voice = FindOrNull(voices_, pitch)) {
    voice->Stop();
  }
}

void DrumkitInstrument::NoteOn(float pitch, float intensity) {
  if (auto* voice = FindOrNull(voices_, pitch)) {
    voice->set_gain(intensity);
    voice->Start();
  }
}

void DrumkitInstrument::Process(float* output, int num_channels,
                                int num_frames) {
  for (int frame = 0; frame < num_frames; ++frame) {
    float mono_sample = 0.0f;
    for (auto& voice : voices_) {
      mono_sample += voice.second.Next(0);
    }
    mono_sample *= gain_;
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void DrumkitInstrument::SetCustomData(std::any data) {
  auto* drumkit_files =
      std::any_cast<std::unordered_map<float, WavFile>*>(data);
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
