#include "barelymusician/instruments/percussion_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

using ::barely::PercussionParameter;

void PercussionInstrument::Process(double* output, int num_channels,
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

void PercussionInstrument::SetData(void* data) noexcept {
  if (data) {
    pads_.swap(*static_cast<PercussionPadMap*>(data));
  } else {
    pads_.clear();
  }
}

void PercussionInstrument::SetNoteOff(double pitch) noexcept {
  if (const auto it = pads_.find(pitch); it != pads_.end()) {
    it->second.voice.Stop();
  }
}

void PercussionInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (const auto it = pads_.find(pitch); it != pads_.end()) {
    it->second.voice.set_gain(intensity);
    it->second.voice.Start();
  }
}

void PercussionInstrument::SetParameter(int index, double value,
                                        double /*slope*/) noexcept {
  switch (static_cast<PercussionParameter>(index)) {
    case PercussionParameter::kRelease:
      for (auto& [pitch, pad] : pads_) {
        pad.voice.envelope().SetRelease(static_cast<double>(value));
      }
      break;
  }
}

Instrument::Definition PercussionInstrument::GetDefinition() noexcept {
  static const std::vector<Parameter::Definition> parameter_definitions = {
      // Pad release.
      Parameter::Definition{0.1, 0.0, 60.0},
  };
  return GetInstrumentDefinition<PercussionInstrument>(parameter_definitions);
}

}  // namespace barelyapi
