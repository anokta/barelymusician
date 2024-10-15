#include "barelymusician/internal/instrument_processor.h"

#include <cassert>
#include <cmath>

#include "barelymusician/barelymusician.h"
#include "barelymusician/dsp/oscillator.h"
#include "barelymusician/internal/sample_data.h"

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
    : reference_frequency_(reference_frequency),
      gain_processor_(frame_rate),
      voice_(frame_rate, kMaxVoiceCount) {}

void InstrumentProcessor::Process(double* output_samples, int output_channel_count,
                                  int output_frame_count) noexcept {
  for (int frame = 0; frame < output_frame_count; ++frame) {
    const double mono_sample = voice_.Next();
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
      voice_.Resize(static_cast<int>(value));
      break;
    case InstrumentControl::kOscillatorType:
      voice_.Update([value](Voice& voice) noexcept {
        voice.oscillator().SetType(static_cast<OscillatorType>(static_cast<int>(value)));
      });
      break;
    case InstrumentControl::kSamplePlayerLoop:
      voice_.Update([value](Voice& voice) noexcept {
        voice.sample_player().SetLoop(static_cast<bool>(value));
      });
      break;
    case InstrumentControl::kAttack:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetAttack(value); });
      break;
    case InstrumentControl::kDecay:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetDecay(value); });
      break;
    case InstrumentControl::kSustain:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetSustain(value); });
      break;
    case InstrumentControl::kRelease:
      voice_.Update([value](Voice& voice) noexcept { voice.envelope().SetRelease(value); });
      break;
    case InstrumentControl::kPitchShift:
      // TODO(#139): Simplify pitch shift.
      if (const double pitch_offset = value - pitch_shift_; pitch_offset != 0.0) {
        pitch_shift_ = value;
        voice_.Update([frequency_ratio = std::pow(2.0, pitch_offset)](Voice& voice) noexcept {
          if (voice.IsActive()) {
            voice.oscillator().SetFrequency(voice.oscillator().GetFrequency() * frequency_ratio);
            voice.sample_player().SetSpeed(voice.sample_player().GetSpeed() * frequency_ratio);
          }
        });
      }
      break;
    case InstrumentControl::kRetrigger:
      voice_.SetRetrigger(static_cast<bool>(value));
      break;
    default:
      assert(false);
      break;
  }
}

void InstrumentProcessor::SetNoteOff(double pitch) noexcept { voice_.Stop(pitch); }

void InstrumentProcessor::SetNoteOn(double pitch, double intensity) noexcept {
  const double frequency = GetFrequency(pitch + pitch_shift_, reference_frequency_);
  const auto* sample_data = sample_data_.Select(pitch);
  const double speed = (sample_data != nullptr && pitch + pitch_shift_ != sample_data->root_pitch)
                           ? frequency / GetFrequency(sample_data->root_pitch, reference_frequency_)
                           : 1.0;
  voice_.Start(pitch, [&](Voice& voice) {
    voice.oscillator().SetFrequency(frequency);
    if (sample_data != nullptr) {
      voice.sample_player().SetData(sample_data->samples, sample_data->sample_rate,
                                    sample_data->sample_count);
      voice.sample_player().SetSpeed(speed);
    }
    voice.set_gain(intensity);
  });
}

void InstrumentProcessor::SetSampleData(SampleData& sample_data) noexcept {
  voice_.Update([](Voice& voice) { voice.sample_player().SetData(nullptr, 0, 0); });
  sample_data_.Swap(sample_data);
  // TODO(#139): Support data update for already playing voices.
}

}  // namespace barely
