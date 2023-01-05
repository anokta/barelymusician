#include "barelymusician/instruments/percussion_instrument.h"

#include <cstddef>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/parameter.h"
#include "barelymusician/instruments/generic_instrument.h"

namespace barely {

PercussionInstrument::PercussionInstrument(int frame_rate) noexcept
    : pads_{Pad(frame_rate), Pad(frame_rate), Pad(frame_rate),
            Pad(frame_rate)} {}

void PercussionInstrument::Process(double* output_samples, int channel_count,
                                   int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    double mono_sample = 0.0;
    for (auto& pad : pads_) {
      mono_sample += pad.voice.Next(0);
    }
    for (int channel = 0; channel < channel_count; ++channel) {
      output_samples[channel_count * frame + channel] = mono_sample;
    }
  }
}

void PercussionInstrument::SetData(const void* data, int /*size*/) noexcept {
  for (int i = 0; i < kPadCount; ++i) {
    if (data) {
      // Pad data is sequentially aligned by pitch, frequency, length and data.
      const double pitch = *reinterpret_cast<const double*>(data);
      data = static_cast<const std::byte*>(data) + sizeof(double);
      const int frequency = *reinterpret_cast<const int*>(data);
      data = static_cast<const std::byte*>(data) + sizeof(int);
      const int length = *reinterpret_cast<const int*>(data);
      data = static_cast<const std::byte*>(data) + sizeof(int);
      const double* voice_data = reinterpret_cast<const double*>(data);
      data = static_cast<const std::byte*>(data) + sizeof(double) * length;
      pads_[i].pitch = pitch;
      pads_[i].voice.generator().SetData(voice_data, frequency, length);
    } else {
      pads_[i].pitch = 0.0;
      pads_[i].voice.generator().SetData(nullptr, 0, 0);
    }
  }
}

void PercussionInstrument::SetNoteOff(double pitch) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.Stop();
      break;
    }
  }
}

void PercussionInstrument::SetNoteOn(double pitch) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      // TODO(#75): Use note parameters instead.
      // pad.voice.set_gain(intensity);
      pad.voice.Start();
      break;
    }
  }
}

void PercussionInstrument::SetParameter(int index, double value,
                                        double /*slope*/) noexcept {
  switch (static_cast<PercussionParameter>(index)) {
    case PercussionParameter::kRelease:
      for (auto& pad : pads_) {
        pad.voice.envelope().SetRelease(static_cast<double>(value));
      }
      break;
  }
}

InstrumentDefinition PercussionInstrument::GetDefinition() noexcept {
  static const std::vector<ParameterDefinition> parameter_definitions = {
      // Pad release.
      ParameterDefinition{0.1, 0.0, 60.0},
  };
  return GetInstrumentDefinition<PercussionInstrument>(parameter_definitions);
}

}  // namespace barely
