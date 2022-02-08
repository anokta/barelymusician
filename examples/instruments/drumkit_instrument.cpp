#include "examples/instruments/drumkit_instrument.h"

#include <unordered_map>

#include "barelymusician/common/find_or_null.h"
#include "examples/common/wav_file.h"
#include "examples/instruments/generic_instrument.h"

namespace barely::examples {

using ::barelyapi::FindOrNull;
using ::barelyapi::InstrumentDefinition;
using ::barelyapi::ParameterDefinition;

DrumkitInstrument::DrumkitInstrument(int sample_rate) noexcept
    : sample_rate_(sample_rate) {}

void DrumkitInstrument::NoteOff(float pitch) noexcept {
  if (auto* pad = FindOrNull(pads_, pitch)) {
    pad->voice.Stop();
  }
}

void DrumkitInstrument::NoteOn(float pitch, float intensity) noexcept {
  if (auto* pad = FindOrNull(pads_, pitch)) {
    pad->voice.set_gain(intensity);
    pad->voice.Start();
  }
}

void DrumkitInstrument::Process(float* output, int num_channels,
                                int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    float mono_sample = 0.0f;
    for (auto& [pitch, pad] : pads_) {
      mono_sample += pad.voice.Next(0);
    }
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void DrumkitInstrument::SetData(void* data) noexcept {
  for (const auto& [pitch, file] :
       *reinterpret_cast<std::unordered_map<float, WavFile>*>(data)) {
    pads_.insert({pitch, DrumkitPad{file, sample_rate_}});
  }
}

void DrumkitInstrument::SetParameter(int index, float value) noexcept {
  switch (static_cast<DrumkitInstrumentParameter>(index)) {
    case DrumkitInstrumentParameter::kPadRelease:
      for (auto& [pitch, pad] : pads_) {
        pad.voice.envelope().SetRelease(value);
      }
      break;
  }
}

InstrumentDefinition DrumkitInstrument::GetDefinition() noexcept {
  return GetInstrumentDefinition<DrumkitInstrument>({
      // Pad release.
      ParameterDefinition{0.1f, 0.0f, 60.0f},
  });
}

}  // namespace barely::examples
