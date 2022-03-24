#include "barelymusician/instruments/drumkit_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

using ::barely::InstrumentDefinition;
using ::barely::ParameterDefinition;
using ::barely::PercussionParameter;

void DrumkitInstrument::Process(double* output, int num_channels,
                                int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    double mono_sample = 0.0;
    for (auto& [pitch, pad] : pads_) {
      mono_sample += pad.voice.Next(0);
    }
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void DrumkitInstrument::SetData(void* data) noexcept {
  if (data) {
    pads_.swap(*static_cast<DrumkitPadMap*>(data));
  } else {
    pads_.clear();
  }
}

void DrumkitInstrument::SetNoteOff(double pitch) noexcept {
  if (const auto it = pads_.find(pitch); it != pads_.end()) {
    it->second.voice.Stop();
  }
}

void DrumkitInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (const auto it = pads_.find(pitch); it != pads_.end()) {
    it->second.voice.set_gain(intensity);
    it->second.voice.Start();
  }
}

void DrumkitInstrument::SetParameter(int index, double value,
                                     double /*slope*/) noexcept {
  switch (static_cast<PercussionParameter>(index)) {
    case PercussionParameter::kRelease:
      for (auto& [pitch, pad] : pads_) {
        pad.voice.envelope().SetRelease(static_cast<double>(value));
      }
      break;
  }
}

InstrumentDefinition DrumkitInstrument::GetDefinition() noexcept {
  static std::vector<ParameterDefinition> parameter_definitions = {
      // Pad release.
      ParameterDefinition{0.1, 0.0, 60.0},
  };
  return GetInstrumentDefinition<DrumkitInstrument>(parameter_definitions);
}

}  // namespace barelyapi
