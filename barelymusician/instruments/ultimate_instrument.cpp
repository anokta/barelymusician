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
          // Sample player loop.
          ControlDefinition{Control::kSamplePlayerLoop, false},
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
    : frame_rate_(frame_rate),
      oscillator_voice_(OscillatorVoice(frame_rate_), kMaxVoiceCount),
      gain_processor_(frame_rate_) {}

void UltimateInstrument::Process(double* output_samples, int output_channel_count,
                                 int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    double mono_sample = (oscillator_on_ ? oscillator_voice_.Next(0) : 0.0);
    for (auto& sampler : samplers_) {
      mono_sample += sampler.voice.Next(0);
    }
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
      voice_count_ = static_cast<int>(value);
      oscillator_voice_.Resize(static_cast<int>(value));
      for (auto& sampler : samplers_) {
        sampler.voice.Resize(static_cast<int>(value));
      }
      break;
    case Control::kOscillatorOn:
      oscillator_on_ = static_cast<bool>(value);
      break;
    case Control::kOscillatorType:
      oscillator_voice_.Update([value](OscillatorVoice* voice) noexcept {
        voice->generator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case Control::kSamplePlayerLoop:
      sampler_loop_ = value;
      for (auto& sampler : samplers_) {
        sampler.voice.Update([value](Sampler::Voice* voice) noexcept {
          voice->generator().SetLoop(static_cast<bool>(value));
        });
      }
      break;
    case Control::kAttack:
      attack_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetAttack(value); });
      }
      break;
    case Control::kDecay:
      decay_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetDecay(value); });
      }
      break;
    case Control::kSustain:
      sustain_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetSustain(value); });
      }
      break;
    case Control::kRelease:
      release_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetRelease(value); });
      }
      break;
    default:
      assert(false);
      break;
  }
}

void UltimateInstrument::SetData(const void* data, int size) noexcept {
  const double* data_double = static_cast<const double*>(data);
  if (data_double == nullptr || size == 0) {
    samplers_.clear();
    return;
  }
  const int sampler_count = static_cast<int>(*data_double++);
  samplers_.resize(sampler_count, Sampler(frame_rate_));
  for (auto& sampler : samplers_) {
    // Sampler data is sequentially aligned by pitch, frequency, length, and data.
    sampler.pitch = *data_double++;
    const int frequency = static_cast<int>(*data_double++);
    const int length = static_cast<int>(*data_double++);
    sampler.voice.Resize(voice_count_);
    sampler.voice.Update([&](Sampler::Voice* voice) noexcept {
      voice->generator().SetData(data_double, frequency, length);
      voice->generator().SetLoop(sampler_loop_);
      voice->envelope().SetAttack(attack_);
      voice->envelope().SetDecay(decay_);
      voice->envelope().SetSustain(sustain_);
      voice->envelope().SetRelease(release_);
    });
    data_double += length;
  }
}

void UltimateInstrument::SetNoteOff(double pitch) noexcept {
  oscillator_voice_.Stop(pitch);
  for (auto& sampler : samplers_) {
    if (samplers_.size() == 1 || sampler.pitch == pitch) {
      sampler.voice.Stop(pitch);
    }
  }
}

void UltimateInstrument::SetNoteOn(double pitch, double intensity) noexcept {
  oscillator_voice_.Start(pitch,
                          [frequency = GetFrequency(pitch), intensity](OscillatorVoice* voice) {
                            voice->generator().SetFrequency(frequency);
                            voice->set_gain(intensity);
                          });
  // TODO(#139): Refactor this to make the percussion vs pitched sample distinction more robust.
  if (samplers_.size() == 1) {
    samplers_.front().voice.Start(
        pitch, [speed = GetFrequency(pitch) / GetFrequency(samplers_.front().pitch),
                intensity](Sampler::Voice* voice) {
          voice->generator().SetSpeed(speed);
          voice->set_gain(intensity);
        });
  } else {
    for (auto& sampler : samplers_) {
      if (sampler.pitch == pitch) {
        sampler.voice.Start(pitch,
                            [intensity](Sampler::Voice* voice) { voice->set_gain(intensity); });
        break;
      }
    }
  }
}

UltimateInstrument::Sampler::Sampler(int frame_rate) noexcept
    : voice(Sampler::Voice(frame_rate), kMaxVoiceCount) {}

}  // namespace barely
