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

BasicDrumkitInstrument::BasicDrumkitInstrument(float sample_interval)
    : sample_interval_(sample_interval), gain_(kDefaultGain) {}

void BasicDrumkitInstrument::NoteOff(float index) {
  const auto it = voices_.find(index);
  if (it == voices_.end()) {
    LOG(WARNING) << "Invalid note index " << index;
    return;
  }
  DLOG(INFO) << "BasicDrumkitInstrument::NoteOff(" << index << ")";
  it->second.Stop();
}

void BasicDrumkitInstrument::NoteOn(float index, float intensity) {
  const auto it = voices_.find(index);
  if (it == voices_.end()) {
    LOG(WARNING) << "Invalid note index " << index;
    return;
  }
  DLOG(INFO) << "BasicDrumkitInstrument::NoteOn(" << index << ", " << intensity
             << ")";
  it->second.SetGain(intensity);
  it->second.Start();
}

void BasicDrumkitInstrument::Process(float* output, int num_channels,
                                     int num_frames) {
  for (int frame = 0; frame < num_frames; ++frame) {
    float sample = 0.0f;
    for (auto& voice : voices_) {
      sample += voice.second.Next();
    }
    sample *= gain_;
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = sample;
    }
  }
}

void BasicDrumkitInstrument::Reset() {
  for (auto& voice : voices_) {
    voice.second.Reset();
  }
}

void BasicDrumkitInstrument::Add(float note_index, const WavFile& wav_file) {
  BasicSamplerVoice voice(sample_interval_);
  voice.SetEnvelopeRelease(kDefaultRelease);
  voice.SetSamplePlayerData(wav_file.GetData(), wav_file.GetSampleRate());
  voices_.insert({note_index, voice});
}

}  // namespace examples
}  // namespace barelyapi
