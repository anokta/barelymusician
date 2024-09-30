#include "barelymusician/instruments/synth_instrument.h"

#include <array>
#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelySynthInstrument_GetDefinition() {
  return barely::SynthInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 64;

}  // namespace

InstrumentDefinition SynthInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{Control::kGain, 1.0, 0.0, 1.0},
          // Oscillator type.
          ControlDefinition{Control::kOscillatorType, static_cast<double>(OscillatorType::kSine),
                            0.0, static_cast<double>(OscillatorType::kNoise)},
          // Attack.
          ControlDefinition{Control::kAttack, 0.05, 0.0, 60.0},
          // Decay.
          ControlDefinition{Control::kDecay, 0.0, 0.0, 60.0},
          // Sustain.
          ControlDefinition{Control::kSustain, 1.0, 0.0, 1.0},
          // Release.
          ControlDefinition{Control::kRelease, 0.25, 0.0, 60.0},
          // Number of voices.
          ControlDefinition{Control::kVoiceCount, 8, 1, kMaxVoiceCount},
      };
  return CustomInstrument::GetDefinition<SynthInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
SynthInstrument::SynthInstrument(int frame_rate) noexcept
    : voice_(SynthVoice(frame_rate), kMaxVoiceCount),
      gain_processor_(frame_rate),
      tuning_(GetStandardTuning()) {}

void SynthInstrument::Process(double* output_samples, int output_channel_count,
                              int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next(0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void SynthInstrument::SetControl(int id, double value) noexcept {
  switch (static_cast<Control>(id)) {
    case Control::kGain:
      gain_processor_.SetGain(value);
      break;
    case Control::kOscillatorType:
      voice_.Update([value](SynthVoice* voice) noexcept {
        voice->generator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case Control::kAttack:
      voice_.Update([value](SynthVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      break;
    case Control::kDecay:
      voice_.Update([value](SynthVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      break;
    case Control::kSustain:
      voice_.Update([value](SynthVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      break;
    case Control::kRelease:
      voice_.Update([value](SynthVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      break;
    case Control::kVoiceCount:
      voice_.Resize(static_cast<int>(value));
      break;
    default:
      assert(false);
      break;
  }
}

void SynthInstrument::SetNoteOff(int pitch) noexcept { voice_.Stop(pitch); }

void SynthInstrument::SetNoteOn(int pitch, double intensity) noexcept {
  voice_.Start(pitch, [frequency = tuning_.GetFrequency(pitch), intensity](SynthVoice* voice) {
    voice->generator().SetFrequency(frequency);
    voice->set_gain(intensity);
  });
}

void SynthInstrument::SetTuning(const TuningDefinition& tuning) noexcept { tuning_ = tuning; }

}  // namespace barely
