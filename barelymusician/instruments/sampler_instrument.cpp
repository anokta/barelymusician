#include "barelymusician/instruments/sampler_instrument.h"

#include <cmath>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instruments/enveloped_voice.h"

namespace barely {

SamplerInstrument::SamplerInstrument(int sample_rate) noexcept
    : voice_(SamplerVoice(sample_rate)), sample_rate_(sample_rate) {}

void SamplerInstrument::Process(double* output, int num_channels,
                                int num_frames) noexcept {
  for (int frame = 0; frame < num_frames; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < num_channels; ++channel) {
      output[num_channels * frame + channel] = mono_sample;
    }
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

void SamplerInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  const double speed = std::pow(2.0, pitch - root_pitch_);
  voice_.Start(pitch, [speed, intensity](SamplerVoice* voice) noexcept {
    voice->generator().SetSpeed(speed);
    voice->set_gain(intensity);
  });
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SamplerInstrument::SetParameter(int index, double value,
                                     double /*slope*/) noexcept {
  switch (static_cast<SamplerParameter>(index)) {
    case SamplerParameter::kRootPitch:
      root_pitch_ = value;
      break;
    case SamplerParameter::kLoop:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->generator().SetLoop(static_cast<bool>(value));
      });
      break;
    case SamplerParameter::kAttack:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SamplerParameter::kDecay:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SamplerParameter::kSustain:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SamplerParameter::kRelease:
      voice_.Update([value](SamplerVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SamplerParameter::kNumVoices:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

InstrumentDefinition SamplerInstrument::GetDefinition() noexcept {
  static const std::vector<ParameterDefinition> parameter_definitions = {
      // Root pitch.
      ParameterDefinition{0.0},
      // Sample player loop.
      ParameterDefinition{false},
      // Attack.
      ParameterDefinition{0.05, 0.0, 60.0},
      // Decay.
      ParameterDefinition{0.0, 0.0, 60.0},
      // Sustain.
      ParameterDefinition{1.0, 0.0, 1.0},
      // Release.
      ParameterDefinition{0.25, 0.0, 60.0},
      // Number of voices.
      ParameterDefinition{8, 1, 64},
  };
  return GetInstrumentDefinition<SamplerInstrument>(parameter_definitions);
}

}  // namespace barely
