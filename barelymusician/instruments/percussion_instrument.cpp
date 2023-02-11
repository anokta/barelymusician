#include "barelymusician/instruments/percussion_instrument.h"

#include <cstddef>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/instruments/custom_instrument.h"

extern "C" {

BarelyInstrumentDefinition BarelyPercussionInstrument_GetDefinition() {
  return barely::PercussionInstrument::GetDefinition();
}

}  // extern "C"

namespace barely {

PercussionInstrument::PercussionInstrument(int frame_rate) noexcept
    : pads_{Pad(frame_rate), Pad(frame_rate), Pad(frame_rate), Pad(frame_rate)},
      gain_processor_(frame_rate) {}

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
  gain_processor_.Process(output_samples, channel_count, frame_count);
}

void PercussionInstrument::SetControl(int index, double value,
                                      double /*slope_per_frame*/) noexcept {
  switch (static_cast<PercussionControl>(index)) {
    case PercussionControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case PercussionControl::kRelease:
      for (auto& pad : pads_) {
        pad.voice.envelope().SetRelease(value);
      }
      break;
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

void PercussionInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  for (auto& pad : pads_) {
    if (pad.pitch == pitch) {
      pad.voice.set_gain(intensity);
      pad.voice.Start();
      break;
    }
  }
}

InstrumentDefinition PercussionInstrument::GetDefinition() noexcept {
  static const std::vector<ControlDefinition> control_definitions = {
      // Gain.
      ControlDefinition{1.0, 0.0, 1.0},
      // Pad release.
      ControlDefinition{0.1, 0.0, 60.0},
  };
  return GetInstrumentDefinition<PercussionInstrument>(control_definitions, {});
}

}  // namespace barely
