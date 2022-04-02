#include "barelymusician/instruments/percussion_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barelyapi {

using ::barely::PercussionParameter;

PercussionInstrument::PercussionInstrument(int frame_rate) noexcept
    : frame_rate_(frame_rate) {}

void PercussionInstrument::Process(double* output, int num_channels,
                                   int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    double mono_sample = 0.0;
    for (auto& [pitch, voice] : voices_) {
      mono_sample += voice.Next(0);
    }
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
  }
}

void PercussionInstrument::SetData(const void* data, int /*size*/) noexcept {
  pads_ = *static_cast<const PercussionPadMap* const*>(data);
  if (pads_) {
    // TODO(#44): Temporary workaround, should be preallocated.
    voices_.clear();
    voices_.reserve(pads_->size());
    for (const auto& [pitch, pad] : *pads_) {
      auto [it, success] =
          voices_.emplace(pitch, EnvelopedVoice<SamplePlayer>(frame_rate_));
      it->second.generator().SetData(pad.data.data(), pad.frequency,
                                     static_cast<int>(pad.data.size()));
    }
  }
}

void PercussionInstrument::SetNoteOff(double pitch) noexcept {
  if (const auto it = voices_.find(pitch); it != voices_.end()) {
    it->second.Stop();
  }
}

void PercussionInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  if (const auto it = voices_.find(pitch); it != voices_.end()) {
    it->second.set_gain(intensity);
    it->second.Start();
  }
}

void PercussionInstrument::SetParameter(int index, double value,
                                        double /*slope*/) noexcept {
  switch (static_cast<PercussionParameter>(index)) {
    case PercussionParameter::kRelease:
      for (auto& [pitch, voice] : voices_) {
        voice.envelope().SetRelease(static_cast<double>(value));
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
