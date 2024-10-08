#include "barelymusician/instruments/ultimate_instrument.h"

#include <array>
#include <cassert>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/instruments/custom_instrument.h"

BarelyInstrumentDefinition BarelyUltimateInstrument_GetDefinition() {
  return barely::UltimateInstrument::GetDefinition();
}

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 64;

}  // namespace

InstrumentDefinition UltimateInstrument::GetDefinition() noexcept {
  static const std::array<ControlDefinition, static_cast<int>(Control::kCount)>
      control_definitions = {
          // Gain.
          ControlDefinition{Control::kGain, 1.0, 0.0, 1.0},
          // Number of voices.
          ControlDefinition{Control::kVoiceCount, 8, 1, kMaxVoiceCount},
          // Oscillator on.
          ControlDefinition{Control::kOscillatorOn, true},
          // Oscillator type.
          ControlDefinition{Control::kOscillatorType, static_cast<double>(OscillatorType::kSine),
                            0.0, static_cast<double>(OscillatorType::kNoise)},
          // Sample player on.
          ControlDefinition{Control::kSamplePlayerOn, false},
          // Sample player loop.
          ControlDefinition{Control::kSamplePlayerLoop, true},
          // Attack.
          ControlDefinition{Control::kAttack, 0.05, 0.0, 60.0},
          // Decay.
          ControlDefinition{Control::kDecay, 0.0, 0.0, 60.0},
          // Sustain.
          ControlDefinition{Control::kSustain, 1.0, 0.0, 1.0},
          // Release.
          ControlDefinition{Control::kRelease, 0.25, 0.0, 60.0},
      };
  return CustomInstrument::GetDefinition<UltimateInstrument>(control_definitions, {});
}

// NOLINTNEXTLINE(bugprone-exception-escape)
UltimateInstrument::UltimateInstrument(int frame_rate) noexcept
    : oscillator_voice_(OscillatorVoice(frame_rate), kMaxVoiceCount),
      sample_player_voice_(SamplePlayerVoice(frame_rate), kMaxVoiceCount),
      gain_processor_(frame_rate) {}

void UltimateInstrument::Process(double* output_samples, int output_channel_count,
                                 int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double oscillator_sample = oscillator_voice_.Next(0);
    const double sample_player_sample = sample_player_voice_.Next(0);
    const double mono_sample = (oscillator_on_ ? oscillator_sample : 0.0) +
                               (sample_player_on_ ? sample_player_sample : 0.0);
    for (int channel = 0; channel < output_channel_count; ++channel) {
      output_samples[output_channel_count * frame + channel] = mono_sample;
    }
  }
  gain_processor_.Process(output_samples, output_channel_count, output_frame_count);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
void UltimateInstrument::SetControl(int id, double value) noexcept {
  switch (static_cast<Control>(id)) {
    case Control::kGain:
      gain_processor_.SetGain(value);
      break;
    case Control::kVoiceCount:
      oscillator_voice_.Resize(static_cast<int>(value));
      sample_player_voice_.Resize(static_cast<int>(value));
      break;
    case Control::kOscillatorOn:
      oscillator_on_ = static_cast<bool>(value);
      break;
    case Control::kOscillatorType:
      oscillator_voice_.Update([value](OscillatorVoice* voice) noexcept {
        voice->generator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case Control::kSamplePlayerOn:
      sample_player_on_ = static_cast<bool>(value);
      break;
    case Control::kSamplePlayerLoop:
      sample_player_voice_.Update([value](SamplePlayerVoice* voice) noexcept {
        voice->generator().SetLoop(static_cast<bool>(value));
      });
      break;
    case Control::kAttack:
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      sample_player_voice_.Update(
          [value](SamplePlayerVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      break;
    case Control::kDecay:
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      sample_player_voice_.Update(
          [value](SamplePlayerVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      break;
    case Control::kSustain:
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      sample_player_voice_.Update(
          [value](SamplePlayerVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      break;
    case Control::kRelease:
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      sample_player_voice_.Update(
          [value](SamplePlayerVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      break;
    default:
      assert(false);
      break;
  }
}

void UltimateInstrument::SetData(const void* data, int size) noexcept {
  const double* data_double = static_cast<const double*>(data);
  sample_player_root_pitch_ = size > 0 ? *data_double++ : 0.0;
  const int frame_rate = size > 0 ? static_cast<int>(*data_double++) : 0;
  const double* sample_data = size > 0 ? data_double : nullptr;
  const int length = size > 0 ? size / static_cast<int>(sizeof(double)) - 2 : 0;
  sample_player_voice_.Update([sample_data, frame_rate, length](SamplePlayerVoice* voice) noexcept {
    voice->generator().SetData(sample_data, frame_rate, length);
  });
}

void UltimateInstrument::SetNoteOff(double pitch) noexcept {
  oscillator_voice_.Stop(pitch);
  sample_player_voice_.Stop(pitch);
}

void UltimateInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  oscillator_voice_.Start(pitch,
                          [frequency = GetFrequency(pitch), intensity](OscillatorVoice* voice) {
                            voice->generator().SetFrequency(frequency);
                            voice->set_gain(intensity);
                          });
  sample_player_voice_.Start(pitch,
                             [speed = GetFrequency(pitch) / GetFrequency(sample_player_root_pitch_),
                              intensity](SamplePlayerVoice* voice) {
                               voice->generator().SetSpeed(speed);
                               voice->set_gain(intensity);
                             });
}

}  // namespace barely
