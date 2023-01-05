#include "barelymusician/instruments/synth_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/enveloped_voice.h"

namespace barely {

SynthInstrument::SynthInstrument(int sample_rate) noexcept
    : voice_(SynthVoice(sample_rate)) {}

void SynthInstrument::Process(double* output_samples, int channel_count,
                              int frame_count) noexcept {
  for (int frame = 0; frame < frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < channel_count; ++channel) {
      output_samples[channel_count * frame + channel] = mono_sample;
    }
  }
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SynthInstrument::SetControl(int index, double value,
                                 double /*slope_per_frame*/) noexcept {
  switch (static_cast<SynthControl>(index)) {
    case SynthControl::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(
            static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case SynthControl::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetAttack(value);
      });
      break;
    case SynthControl::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetDecay(value);
      });
      break;
    case SynthControl::kSustain:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetSustain(value);
      });
      break;
    case SynthControl::kRelease:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->envelope().SetRelease(value);
      });
      break;
    case SynthControl::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
  }
}

void SynthInstrument::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::SetNoteOn(double pitch) noexcept {
  voice_.Start(pitch, [pitch](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    // TODO(#75): Use note controls instead.
    voice->set_gain(1.0);
  });
}

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static const std::vector<ControlDefinition> control_definitions = {
      // Oscillator type.
      ControlDefinition{static_cast<double>(OscillatorType::kSine), 0.0,
                        static_cast<double>(OscillatorType::kNoise)},
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
  return GetInstrumentDefinition<SynthInstrument>(control_definitions, {});
}

}  // namespace barely
