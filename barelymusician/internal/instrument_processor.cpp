#include "barelymusician/internal/instrument_processor.h"

#include <array>
#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"

namespace barely {

namespace {

// Maximum number of voices allowed to be set.
constexpr int kMaxVoiceCount = 32;

// Returns the frequency of a given pitch.
//
// @param pitch Note pitch.
// @param reference_frequency Reference frequency in hertz.
// @return Note frequency.
double GetFrequency(double pitch, double reference_frequency) noexcept {
  return reference_frequency * std::pow(2.0, pitch);
}

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
InstrumentProcessor::InstrumentProcessor(int frame_rate, double reference_frequency) noexcept
    : frame_rate_(frame_rate),
      reference_frequency_(reference_frequency),
      oscillator_voice_(OscillatorVoice(frame_rate_), kMaxVoiceCount),
      gain_processor_(frame_rate_) {}

void InstrumentProcessor::Process(double* output_samples, int output_channel_count,
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
void InstrumentProcessor::SetControl(int id, double value) noexcept {
  switch (static_cast<InstrumentControl>(id)) {
    case InstrumentControl::kGain:
      gain_processor_.SetGain(value);
      break;
    case InstrumentControl::kVoiceCount:
      voice_count_ = static_cast<int>(value);
      oscillator_voice_.Resize(static_cast<int>(value));
      for (auto& sampler : samplers_) {
        sampler.voice.Resize(static_cast<int>(value));
      }
      break;
    case InstrumentControl::kOscillatorOn:
      oscillator_on_ = static_cast<bool>(value);
      break;
    case InstrumentControl::kOscillatorType:
      oscillator_voice_.Update([value](OscillatorVoice* voice) noexcept {
        voice->generator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case InstrumentControl::kSamplePlayerLoop:
      sampler_loop_ = value;
      for (auto& sampler : samplers_) {
        sampler.voice.Update([value](Sampler::Voice* voice) noexcept {
          voice->generator().SetLoop(static_cast<bool>(value));
        });
      }
      break;
    case InstrumentControl::kAttack:
      attack_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetAttack(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetAttack(value); });
      }
      break;
    case InstrumentControl::kDecay:
      decay_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetDecay(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetDecay(value); });
      }
      break;
    case InstrumentControl::kSustain:
      sustain_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetSustain(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetSustain(value); });
      }
      break;
    case InstrumentControl::kRelease:
      release_ = value;
      oscillator_voice_.Update(
          [value](OscillatorVoice* voice) noexcept { voice->envelope().SetRelease(value); });
      for (auto& sampler : samplers_) {
        sampler.voice.Update(
            [value](Sampler::Voice* voice) noexcept { voice->envelope().SetRelease(value); });
      }
      break;
    case InstrumentControl::kPitchShift:
      // TODO(#139): Simplify pitch shift.
      if (const double pitch_offset = value - pitch_shift_; pitch_offset != 0.0) {
        pitch_shift_ = value;
        const double frequency_ratio = std::pow(2.0, pitch_offset);
        oscillator_voice_.Update([frequency_ratio](OscillatorVoice* voice) noexcept {
          if (voice->IsActive()) {
            voice->generator().SetFrequency(voice->generator().GetFrequency() * frequency_ratio);
          }
        });
        if (samplers_.size() == 1) {
          samplers_.front().voice.Update([frequency_ratio](Sampler::Voice* voice) noexcept {
            if (voice->IsActive()) {
              voice->generator().SetSpeed(voice->generator().GetSpeed() * frequency_ratio);
            }
          });
        }
      }
      break;
    default:
      assert(false);
      break;
  }
}

void InstrumentProcessor::SetData(const void* data, int size) noexcept {
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

void InstrumentProcessor::SetNoteOff(double pitch) noexcept {
  oscillator_voice_.Stop(pitch);
  for (auto& sampler : samplers_) {
    if (samplers_.size() == 1 || sampler.pitch == pitch) {
      sampler.voice.Stop(pitch);
    }
  }
}

void InstrumentProcessor::SetNoteOn(double pitch, double intensity) noexcept {
  oscillator_voice_.Start(pitch,
                          [frequency = GetFrequency(pitch + pitch_shift_, reference_frequency_),
                           intensity](OscillatorVoice* voice) {
                            voice->generator().SetFrequency(frequency);
                            voice->set_gain(intensity);
                          });
  // TODO(#139): Refactor this to make the percussion vs pitched sample distinction more robust.
  if (samplers_.size() == 1) {
    samplers_.front().voice.Start(
        pitch, [speed = GetFrequency(pitch + pitch_shift_, reference_frequency_) /
                        GetFrequency(samplers_.front().pitch, reference_frequency_),
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

InstrumentProcessor::Sampler::Sampler(int frame_rate) noexcept
    : voice(Sampler::Voice(frame_rate), kMaxVoiceCount) {}

}  // namespace barely
