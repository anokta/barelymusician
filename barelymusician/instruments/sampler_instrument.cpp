#include "barelymusician/instruments/sampler_instrument.h"

#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/enveloped_voice.h"

namespace barely {

SamplerInstrument::SamplerInstrument(int sample_rate) noexcept
    : voice_(SamplerVoice(sample_rate)), sample_rate_(sample_rate) {}

void SamplerInstrument::Process(double* output_samples, int channel_count,
                                int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < channel_count; ++channel) {
      output_samples[channel_count * frame + channel] = mono_sample;
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SamplerInstrument::SetControl(int index, double value,
                                   double /*slope*/) noexcept {
  switch (static_cast<SamplerControl>(index)) {
    case SamplerControl::kRootPitch:
      root_pitch_ = value;
      break;
    case SamplerControl::kLoop:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->generator().SetLoop(static_cast<bool>(value));
      });
      break;
    case SamplerControl::kAttack:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SamplerControl::kDecay:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SamplerControl::kSustain:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SamplerControl::kRelease:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SamplerControl::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

void SamplerInstrument::SetData(const void* data, int size) noexcept {
  voice_.Update([sample_rate = sample_rate_, data,
                 size](SamplerVoice* voice) noexcept {
    voice->generator().SetData(static_cast<const double*>(data), sample_rate,
                               size / static_cast<int>(sizeof(double)));
  });
}

void SamplerInstrument::SetNoteOff(double pitch) noexcept {
  voice_.Stop(pitch);
}

void SamplerInstrument::SetNoteOn(double pitch) noexcept {
  const double speed = std::pow(2.0, pitch - root_pitch_);
  voice_.Start(pitch, [speed](SamplerVoice* voice) noexcept {
    voice->generator().SetSpeed(speed);
    // TODO(#75): Use note controls instead.
    // voice->set_gain(intensity);
  });
}

InstrumentDefinition SamplerInstrument::GetDefinition() noexcept {
  static const std::vector<ControlDefinition> control_definitions = {
      // Root pitch.
      ControlDefinition{0.0},
      // Sample player loop.
      ControlDefinition{false},
      // Attack.
      ControlDefinition{0.05, 0.0, 60.0},
      // Decay.
      ControlDefinition{0.0, 0.0, 60.0},
      // Sustain.
      ControlDefinition{1.0, 0.0, 1.0},
      // Release.
      ControlDefinition{0.25, 0.0, 60.0},
      // Number of voices.
      ControlDefinition{8, 1, 64},
  };
  return GetInstrumentDefinition<SamplerInstrument>(control_definitions);
}

}  // namespace barely
