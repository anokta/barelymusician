#include "instruments/basic_drumkit_instrument.h"

#include <algorithm>

#include "barelymusician/base/logging.h"

namespace barelyapi {
namespace examples {

namespace {

// Default values.
const float kDefaultGain = 0.5f;
const float kDefaultRelease = 0.1f;

}  // namespace

BasicDrumkitInstrument::BasicDrumkitInstrument(int sample_rate)
    : sample_rate_(sample_rate), gain_(kDefaultGain) {}

void BasicDrumkitInstrument::NoteOff(float index) {
  const auto it = voices_.find(index);
  if (it == voices_.cend()) {
    LOG(WARNING) << "Invalid note index " << index;
    return;
  }
  it->second.Stop();
}

void BasicDrumkitInstrument::NoteOn(float index, float intensity) {
  const auto it = voices_.find(index);
  if (it == voices_.cend()) {
    LOG(WARNING) << "Invalid note index " << index;
    return;
  }
  it->second.set_gain(intensity);
  it->second.Start();
}

void BasicDrumkitInstrument::Process(float* output, int num_channels,
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

void BasicDrumkitInstrument::Add(float note_index, const WavFile& wav_file) {
  BasicDrumkitVoice voice(sample_rate_);
  voice.envelope().SetRelease(kDefaultRelease);
  const auto& data = wav_file.GetData();
  const int data_size = static_cast<int>(data.size());
  voice.generator().SetData(data.data(), wav_file.GetSampleRate(), data_size);
  voices_.insert({note_index, voice});
}

}  // namespace examples
}  // namespace barelyapi
