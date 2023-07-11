#include "barelymusician/instruments/synth_instrument.h"

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/dsp_utils.h"
#include "barelymusician/dsp/enveloped_voice.h"
#include "barelymusician/dsp/oscillator.h"

extern "C" {

BarelyInstrumentDefinition BarelySynthInstrument_GetDefinition() {
  return barely::SynthInstrument::GetDefinition();
}

}  // extern "C"

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 64;

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static const std::vector<ControlDefinition> control_definitions = {
      // Gain.
      ControlDefinition{1.0, 0.0, 1.0},
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
      ControlDefinition{8, 1, kMaxVoiceCount},
  };
  return CustomInstrument::GetDefinition<SynthInstrument>(control_definitions,
                                                          {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
SynthInstrument::SynthInstrument(int frame_rate) noexcept
    : voice_(SynthVoice(frame_rate), kMaxVoiceCount),
      gain_processor_(frame_rate) {}

void SynthInstrument::Process(double* output_samples, int output_channel_count,
                              int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count,
                          output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SynthInstrument::SetControl(int index, double value,
                                 double /*slope_per_frame*/) noexcept {
  switch (static_cast<SynthControl>(index)) {
    case SynthControl::kGain:
      gain_processor_.SetGain(value);
      break;
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

void SynthInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  voice_.Start(pitch, [pitch, intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(GetFrequency(pitch));
    voice->set_gain(intensity);
  });
}

}  // namespace barely
